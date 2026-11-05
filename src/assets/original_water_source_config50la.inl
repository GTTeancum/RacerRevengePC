//Included in the resolver TU after Fresh and the existing numeric I/O owner.
bool OriginalWaterSourceConfig50LA::compareName308F00(std::string_view a,std::string_view b,bool& equal)noexcept {
    auto& flowPC=fieldFlowPCNative();
    using G=OriginalConfigProcessGlobals50LA;const G::WaterSprintfAccess key;
    for(std::size_t i=0;;++i){
        const auto x=i<a.size()?static_cast<unsigned char>(a[i]):0u;
        const auto y=i<b.size()?static_cast<unsigned char>(b[i]):0u;
        std::int32_t fx=0,fy=0;
        if(x){flowPC=0x308f34;if(!G::waterFold308F00(key,x,fx,flowPC))return false;
            flowPC=0x308f3c;if(!G::waterFold308F00(key,y,fy,flowPC))return false;
            if(fx==fy)continue;}
        flowPC=0x308f90;if(!G::waterFold308F00(key,x,fx,flowPC))return false;
        flowPC=0x308f94;if(!G::waterFold308F00(key,y,fy,flowPC))return false;
        equal=fx==fy;return true;
    }
}
bool OriginalWaterSourceConfig50LA::read12FB30(Read& read,std::size_t size,std::uint32_t resume,D& d)noexcept {
    if(read.attempted||!borrowed_||handle_<0||!resolver_||
       !resolver_->sourceBankMemberCurrent(selectedRoot_,this,handle_))return fail(d,E::phase,pc_);
    read.attempted=true;pc_=0x12fb30;
    read.before=resolver_->freshFileCapacity_->members150[handle_].cursor10;
    read.returned=resolver_->readSourceBankMember12FB30(selectedRoot_,this,handle_,read.bytes.data(),size,read.copied,d);
    read.after=resolver_->freshFileCapacity_->members150[handle_].cursor10;expectedCursor_=read.after;
    if(!read.returned){pc_=d.sourcePC?d.sourcePC:pc_;return false;}
    //Source size7/4,count1 ignores v0. copied is native byte knownness, NOT
    //the original fread item return. Never initialize a short-read suffix.
    pc_=resume;return true;
}
bool OriginalWaterSourceConfig50LA::rejectBinary157968(std::uint32_t closePC,D& d)noexcept {
    pc_=closePC;rejectionCloseAttempted_=true;
    if(!resolver_->closeSourceBankMember131040(selectedRoot_,this,handle_,false,d)){
        pc_=d.sourcePC?d.sourcePC:pc_;return false;}
    rejectionCloseReturned_=true;pc_=closePC==0x157970u?0x15797cu:0x1579c0u;
    source_.file00=-1;handle_=-1;selectedRoot_=nullptr;
    openReturned_=false;pc_=0x157ae0;return true;
}
bool OriginalWaterSourceConfig50LA::begin1577D0(
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver,
    std::shared_ptr<OriginalStandardArchiveProvider50LA> provider,
    std::shared_ptr<OriginalRaTrackSourceFile50LA> track,const std::string& filename,D& d)noexcept {
    d={};if(attempted_||nativeClosed_||!waterReceiver_||thread_!=std::this_thread::get_id()||
       !resolver||!provider||!track||!provider->sealed()||!resolver->freshFileCapacity_||!resolver->scratch_||
       filename.empty()||filename.find('\0')!=std::string::npos)return fail(d,E::phase,0x2b9324);
    auto& s=*resolver->freshFileCapacity_;
    if(s.provider!=provider||s.track.root!=track.get()||!s.track.joined||!s.track.cacheReturned||
       s.waterConfigBorrower||!s.members150||s.memberCount154<=0||s.cursor168< -1||s.cursor168>=s.memberCount154)
        return fail(d,E::phase,0x2b9324);
    try{filename_=filename;}catch(...){return fail(d,E::allocation,0x2b9324);}
    resolver_=std::move(resolver);provider_=std::move(provider);track_=std::move(track);
    attempted_=true;borrowed_=true;s.waterConfigBorrower=this;trackGeneration_=s.track.generation;
    //Fresh ctor makes every1577D0 old text/handle/index/block cleanup arm skip.
    //a2=0 selects the actual3A62A0 "rb" branch at1578C0.
    pc_=0x1578d0;memberCount_=s.memberCount154;before168_=s.cursor168;
    ++s.cursor168;if(s.cursor168==s.memberCount154)s.cursor168=0;
    const auto start=static_cast<std::int32_t>(s.cursor168);auto index=start;
    do{const auto& row=s.members150[index];bool equal=false;
        if(row.filename08&&!compareName308F00(filename_,row.filename08,equal))return false;
        ++rowsCompared_;if(row.filename08&&equal){found_=true;s.cursor168=index;break;}
        if(++index==s.memberCount154)index=0;
    }while(index!=start);
    after168_=s.cursor168;searchReturned_=true;d.sourceEffectsCommitted=true;
    if(!found_){
        pc_=0x131260;missMode170_=s.sourceFileMode170;
        if(*missMode170_){pc_=0x12dcb0;return fail(d,E::unsupported,pc_);}
        managerHandle_=-1;managerReturned_=true;pc_=0x1578d8;source_.file00=-1;
        openReturned_=false;pc_=0x157ae0;return true;
    }
    //Do not skip a global winner in another bank to manufacture a Track hit.
    pc_=0x131380;const auto device=s.members150[index].deviceID00;
    if(device==s.track.device)selectedRoot_=s.track.root;
    else if(device==s.courseDevice&&s.courseRootOwner&&
        s.courseStage==OriginalStandardTextureResolver50LA::FreshFileCapacity::CourseStage::joined)selectedRoot_=s.courseRootOwner;
    else return fail(d,E::unsupported,pc_);
    if(!resolver_->openSourceBankMember131380(selectedRoot_,this,index,handle_,d)){
        pc_=d.sourcePC?d.sourcePC:pc_;return false;}
    //131230 reloads the selected index at13135C, not131380's v0.
    managerHandle_=index;managerReturned_=true;pc_=0x1578d8;source_.file00=index;
    pc_=0x1578f8;source_.field42C=0;pc_=0x1578fc;source_.opened0C=1;pc_=0x157900;source_.mode10=0;
    pc_=0x157914;if(!read12FB30(magic_,7,0x15791c,d))return false;
    pc_=0x15792c;magic_.bytes[6]=0;magicTerminated_=true;
    //Bounded byte-result strcmp projection. No EE alignment/word-overread
    //or unknown stack byte is fabricated. An early known mismatch suffices.
    pc_=0x3090e8;constexpr char literal[]="BINARY";
    for(std::size_t i=0;i<7;++i){
        if(i>=magic_.copied&&i!=6)return fail(d,E::unsupported,pc_);
        const auto x=magic_.bytes[i],y=static_cast<std::uint8_t>(literal[i]);
        if(x!=y||!x){magicCompare_=std::int32_t(x)-std::int32_t(y);break;}
    }
    pc_=0x157930;
    if(*magicCompare_){
        pc_=0x157a9c;source_.binary08=0;pc_=0x157ab0;seekAttempted_=true;
        if(!resolver_->seekSourceBankMember12F640(selectedRoot_,this,handle_,0,d)){
            pc_=d.sourcePC?d.sourcePC:pc_;return false;}
        expectedCursor_=0;seekReturned_=true;pc_=0x157abc;textCallReached_=true;
        pc_=0x157500;if(!parseText157500(d))return false;
        pc_=0x157ac4;source_.field428=0;pc_=0x157ac8;openReturned_=true;
        pc_=0x157ae0;return true;
    }
    pc_=0x15793c;source_.binary08=1;pc_=0x157950;
    if(!read12FB30(version_,4,0x157958,d))return false;
    if(version_.copied<4)return fail(d,E::unsupported,pc_);
    std::uint32_t word=0;for(unsigned i=0;i<4;++i)word|=std::uint32_t(version_.bytes[i])<<(8*i);
    versionWord_=word;pc_=0x157960;
    if(word!=1u)return rejectBinary157968(0x157970,d);
    pc_=0x157998;if(!read12FB30(count_,4,0x1579a0,d))return false;
    if(count_.copied<4)return fail(d,E::unsupported,pc_);
    word=0;for(unsigned i=0;i<4;++i)word|=std::uint32_t(count_.bytes[i])<<(8*i);
    countWord_=word;pc_=0x1579a4;
    if(!word)return rejectBinary157968(0x1579b4,d);
    pc_=0x1579cc;source_.count14=word;
    //Source count*1024 wraps to signed32 before305700. Retain the real
    //field store, but do not allocate or invent malloc/new-handler outcomes.
    allocationLow32_=word<<10u;
    allocationArgument_=*allocationLow32_<=0x7fffffffu?std::int64_t(*allocationLow32_):
        std::int64_t(*allocationLow32_)-0x100000000LL;
    pc_=0x305700;return false;
}
bool OriginalWaterSourceConfig50LA::closeRejectedStack1573B0(D& d)noexcept {
    d={};if(!openReturned_||*openReturned_||destructorEntered_||source_.file00!=-1||handle_!=-1||
       source_.text1C||source_.child24||source_.cacheCount438||source_.names4CC||source_.index4BC||source_.block4C4)
        return fail(d,E::phase,0x1573b0);
    destructorEntered_=true;pc_=0x15740c; //file00=-1 skips flush,close,SD.
    pc_=0x157ce4;source_.selected20=0;pc_=0x157ce8;source_.child24=nullptr;selectionCleared_=true;
    pc_=0x157478;source_.cacheCount438=0;
    //All nullable arrays are genuinely NULL. a1=-1 is signed16 negative,
    //so1574C8 skips delete2E8B00. No source free/handle close is fabricated.
    pc_=0x1574f4;destructorReturned_=true;return true;
}
bool OriginalWaterSourceConfig50LA::preflightNative(const void* receiver)const noexcept {
    if(!preflightTextNative()||!preflightSectionNative()||!preflightFloatGettersNative()||!preflightIntegerGettersNative()||!preflightShaderGetterNative()||!preflightWakeSelectionNative()||!preflightWakeRetirementNative()||!preflightWakeLoadNative()||!preflightWakeGettersNative()||!preflightArraySelectionNative()||!preflightArrayRetirementNative()||!preflightArrayLoadNative()||!preflightArrayFilenameNative()||!preflightArrayScalarsNative())return false;
    const auto& s=source_;
    if(!receiver||waterReceiver_!=receiver||thread_!=std::this_thread::get_id()||
       s.binary08>1u||s.opened0C>1u||s.mode10||
       s.index18!=(textEntered_?textTable_.capacity18:0u)||
       s.text1C!=(textEntered_?textTable_.sourceStorage:nullptr)||
       s.selected20!=currentSectionCountNative()||s.child24!=currentSectionChildNative()||
       s.field428||s.field42C||s.field430||s.field434||s.cacheCount438||s.names4CC||s.index4BC||
       s.indexBytes4C0||s.block4C4||s.field4C8)return false;
    for(const auto word:s.cache43C)if(word)return false;
    if((borrowed_&&!attempted_)||(searchReturned_&&(!before168_||!after168_))||
       (found_&&!searchReturned_)||managerReturned_!=bool(managerHandle_)||
       (managerReturned_&&!searchReturned_)||(openReturned_&&(!managerReturned_||(*openReturned_&&!textReturned_)))||
       (destructorEntered_&&(!openReturned_||*openReturned_))||selectionCleared_!=destructorReturned_||
       destructorEntered_!=destructorReturned_||seekReturned_!=textCallReached_||
       (seekReturned_&&!seekAttempted_)||rejectionCloseReturned_!=bool(openReturned_&&!*openReturned_&&found_)||
       (rejectionCloseReturned_&&!rejectionCloseAttempted_))return false;
    if(before168_&&(*before168_<-1||*before168_>=memberCount_||rowsCompared_>std::uint32_t(memberCount_)))return false;
    if(searchReturned_){
        const auto start=*before168_+1==memberCount_?0:*before168_+1;
        if(!rowsCompared_||*after168_!=(found_?(start+rowsCompared_-1)%memberCount_:start)||
           (!found_&&rowsCompared_!=std::uint32_t(memberCount_)))return false;
    }
    if(managerHandle_&&((*managerHandle_<0)!=!found_||(!found_&&*managerHandle_!=-1)))return false;
    if(s.opened0C!=(managerReturned_&&found_?1u:0u)||
       s.file00!=(rejectionCloseReturned_?-1:managerHandle_.value_or(-1)))return false;
    if(magicTerminated_&&!magic_.returned)return false;
    if(magicCompare_&&!magicTerminated_)return false;
    if(s.binary08!=(magicCompare_&&!*magicCompare_?1u:0u))return false;
    if(s.count14!=(textEntered_?textTable_.count14:countWord_.value_or(0u)))return false;
    std::optional<std::size_t> cursor;
    const Read* previous=nullptr;
    for(const auto* read:{&magic_,&version_,&count_}){
        const auto size=read==&magic_?7u:4u;
        if(read->copied>size||(read->attempted&&(!managerReturned_||!found_||!read->before||!read->after))||
           (read->returned&&!read->attempted)||
           (read->attempted&&previous&&(!previous->returned||read->before!=previous->after))||
           (read->returned&&*read->after!=*read->before+read->copied))return false;
        if(read->after)cursor=read->after;previous=read;
    }
    if(versionWord_&&(!version_.returned||version_.copied<4))return false;
    if(countWord_&&(!count_.returned||count_.copied<4||versionWord_!=1u))return false;
    if(bool(allocationLow32_)!=bool(allocationArgument_)||
       bool(allocationArgument_)!=bool(countWord_&&*countWord_))return false;
    if(allocationLow32_&&(*allocationLow32_!=(*countWord_<<10u)||
       *allocationArgument_!=(*allocationLow32_<=0x7fffffffu?std::int64_t(*allocationLow32_):
           std::int64_t(*allocationLow32_)-0x100000000LL)))return false;
    if(seekReturned_)cursor=0;if(textEntered_&&textRead_.after)cursor=textRead_.after;
    if(sectionCursor_)cursor=sectionCursor_;if(wakeSelection_.cursor)cursor=wakeSelection_.cursor;
    if(nativeWakeLoad_.cursor)cursor=nativeWakeLoad_.cursor;
    if(nativeArraySelection_.cursor)cursor=nativeArraySelection_.cursor;
    if(nativeArrayLoad_.cursor)cursor=nativeArrayLoad_.cursor;
    if(expectedCursor_!=cursor)return false;
    if(nativeClosed_)return !borrowed_&&!resolver_&&!provider_&&!track_&&!selectedRoot_&&handle_==-1;
    if(!attempted_)return !borrowed_&&!resolver_&&!provider_&&!track_&&handle_==-1;
    if(!borrowed_||!resolver_||!provider_||!track_||!resolver_->freshFileCapacity_||!resolver_->scratch_)return false;
    const auto& f=*resolver_->freshFileCapacity_;
    if(f.waterConfigBorrower!=this||f.provider!=provider_||!provider_->sealed()||
       f.track.root!=track_.get()||!f.track.joined||f.track.generation!=trackGeneration_)return false;
    if(handle_>=0)return selectedRoot_&&resolver_->sourceBankMemberCurrent(selectedRoot_,this,handle_)&&
        (!expectedCursor_||f.members150[handle_].cursor10==*expectedCursor_);
    return !selectedRoot_||(!managerReturned_&&found_); //Selected before a refused open.
}
bool OriginalWaterSourceConfig50LA::abortNative(D& d)noexcept {
    d={};if(!preflightNative(waterReceiver_))return fail(d,E::phase,0);if(nativeClosed_)return true;
    if(borrowed_){auto& s=*resolver_->freshFileCapacity_;
        if(handle_>=0){auto& row=s.members150[handle_];
            if(row.courseConfigOwner!=this||!(row.flags18&4)||row.associatedObject1C)return fail(d,E::phase,0);
            if(row.auxiliarySlot04>=0&&std::size_t(row.auxiliarySlot04)<resolver_->scratch_->slots.size()&&
               resolver_->scratch_->slots[row.auxiliarySlot04].status08==handle_)
                resolver_->scratch_->selected148=row.auxiliarySlot04;
            row.flags18&=~std::uint32_t(4);row.courseConfigOwner=nullptr;
        }
        s.waterConfigBorrower=nullptr;borrowed_=false;
    }
    //Native file/track custody withdrawal only. No1573B0, source file00=-1,
    //parser return or shared search/window rollback is claimed here.
    handle_=-1;selectedRoot_=nullptr;resolver_.reset();provider_.reset();track_.reset();nativeClosed_=true;return true;
}

#include "original_water_source_text50la.inl"

#include "original_water_source_section50la.inl"

#include "original_water_source_getter50la.inl"

#include "original_water_source_integer50la.inl"

#include "original_water_source_shader50la.inl"

#include "original_water_source_wake50la.inl"

#include "original_water_source_retirement50la.inl"

#include "original_water_source_wake_load50la.inl"

#include "original_water_source_wake_getters50la.inl"

#include "original_water_source_array_selection50la.inl"

#include "original_water_source_array_retirement50la.inl"

#include "original_water_source_array_load50la.inl"

#include "original_water_source_array_filename50la.inl"

#include "original_water_source_array_scalars50la.inl"

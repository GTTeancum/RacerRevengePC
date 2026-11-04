// Included in resolver translation unit after Fresh and shared numeric I/O.
bool OriginalParticleSourceConfig50LA::current()const noexcept {
    return !nativeClosed_&&!sourceClosed_&&borrowed_&&thread_==std::this_thread::get_id()&&resolver_&&
        resolver_->freshFileCapacity_&&resolver_->freshFileCapacity_->particleConfigBorrower==this&&
        resolver_->freshFileCapacity_->provider==provider_&&source_->file00==handle_&&handle_>=0&&
        resolver_->sourceBankMemberCurrent(selectedRoot_,this,handle_);
}
bool OriginalParticleSourceConfig50LA::readLine(Line& line,D& d)noexcept {
    line.present=false;line.position=0;if(!current())return fail(d,E::phase,0x12EE90);
    const auto& row=resolver_->freshFileCapacity_->members150[handle_];
    const auto requested=std::min<std::size_t>(row.length14-row.cursor10,1022);
    for(std::size_t i=0;i<=requested;++i)line.known[i]=false;
    if(!resolver_->readSourceBankLine12EE90(selectedRoot_,this,handle_,line.bytes,line.present,line.position,d))return false;
    for(std::size_t i=0;i<=requested;++i)line.known[i]=true;
    return true;
}
bool OriginalParticleSourceConfig50LA::tell(std::int64_t& out,D& d)noexcept {
    if(!current())return fail(d,E::phase,0x12F520);
    std::size_t at=0;if(!resolver_->tellSourceBankMember12F520(selectedRoot_,this,handle_,at,d))return false;
    if(at>0xffffffffULL)return fail(d,E::phase,0x12F630);
    out=at<=0x7fffffffULL?std::int64_t(at):std::int64_t(at)-0x100000000LL;return true;
}
bool OriginalParticleSourceConfig50LA::begin1617A0(
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver,
    std::shared_ptr<OriginalStandardArchiveProvider50LA> provider,const std::string& filename,D& d)noexcept {
    if(occurrence_!=Occurrence::manager1617A0||emitterOwner_)return fail(d,E::phase,0x161824);
    return beginFile1577D0(std::move(resolver),std::move(provider),filename,0x161824,d);
}
bool OriginalParticleSourceConfig50LA::beginEmitter15AE00(
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver,
    std::shared_ptr<OriginalStandardArchiveProvider50LA> provider,const std::string& filename,
    const void* actualEmitter,D& d)noexcept {
    if(occurrence_!=Occurrence::emitter15AE00||!actualEmitter||emitterOwner_!=actualEmitter||
       !emitterForcedSets_||emitterForcedSets_->emitter_!=actualEmitter||!emitterForcedSets_->pointer1C4_)
        return fail(d,E::phase,0x15ae2c);
    return beginFile1577D0(std::move(resolver),std::move(provider),filename,0x15ae2c,d);
}
bool OriginalParticleSourceConfig50LA::beginFile1577D0(
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver,
    std::shared_ptr<OriginalStandardArchiveProvider50LA> provider,const std::string& filename,
    std::uint32_t callerPC,D& d)noexcept {
    d={};if(attempted_||nativeClosed_||thread_!=std::this_thread::get_id()||!resolver||!provider||
        !provider->sealed()||!resolver->freshFileCapacity_||!resolver->scratch_)return fail(d,E::phase,callerPC);
    auto& s=*resolver->freshFileCapacity_;
    //Only the occurrence-qualified private wrappers enter after coldWorld
    //proves its actual filename and caller phase. SAME live file manager,
    //provider and Track registration; no mount/name bytes certify the caller.
    if(s.provider!=provider||!s.track.root||!s.track.joined||!s.track.cacheReturned||
       s.particleConfigBorrower||s.sourceFileMode170!=0||filename.empty()||filename.find('\0')!=std::string::npos)
        return fail(d,E::phase,callerPC);
    try {filename_=filename;}catch(...){return fail(d,E::allocation,callerPC);}
    resolver_=std::move(resolver);provider_=std::move(provider);attempted_=true;
    s.particleConfigBorrower=this;borrowed_=true;sourcePC_=0x1578D0;
    //131230 global rotating search: no caller-selected bank or forced ordinal.
    if(!s.members150||s.memberCount154<=0||s.cursor168< -1||s.cursor168>=s.memberCount154)
        return fail(d,E::phase,0x1315B0);
    ++s.cursor168;if(s.cursor168==s.memberCount154)s.cursor168=0;
    auto index=static_cast<std::int32_t>(s.cursor168);const auto start=index;bool found=false;
    do {const auto& row=s.members150[index];
        if(row.filename08&&courseSourceName308F00(filename_,row.filename08)){found=true;break;}
        if(++index==s.memberCount154)index=0;
    }while(index!=start);
    d.sourceEffectsCommitted=true;
    if(!found){sourceOpenReturned_=false;sourcePC_=0x157ACC;return true;}
    s.cursor168=index;
    //The established numeric reader currently has exact Course/Track custody.
    //A different globally selected bank is bounded AFTER search, not skipped
    //to force a later desired filename. No POD/nativeGarage admission is added.
    const auto device=s.members150[index].deviceID00;
    if(device==s.track.device&&s.track.joined)selectedRoot_=s.track.root;
    else if(device==s.courseDevice&&s.courseRootOwner&&
        s.courseStage==OriginalStandardTextureResolver50LA::FreshFileCapacity::CourseStage::joined)
        selectedRoot_=s.courseRootOwner;
    else {sourcePC_=0x131380;return fail(d,E::phase,sourcePC_);}
    if(!resolver_->openSourceBankMember131380(selectedRoot_,this,index,handle_,d))return false;
    source_->file00=handle_;sourcePC_=0x1578D8;
    source_->field42C=0;source_->opened0C=1;source_->mode10=0;
    std::array<std::uint8_t,7> magic;std::size_t done=0;sourcePC_=0x157914;
    if(!resolver_->readSourceBankMember12FB30(selectedRoot_,this,handle_,magic.data(),7,done,d))return false;
    sourcePC_=0x15791C;if(done!=7)return false; //Unwritten magic tail remains unknown.
    magic[6]=0;
    if(std::memcmp(magic.data(),"BINARY",6)==0){
        source_->binary08=1;sourcePC_=0x157950;return false;
        //Binary version/count/allocations not yet joined for this occurrence.
    }
    source_->binary08=0;sourcePC_=0x157AB0;
    if(!resolver_->seekSourceBankMember12F640(selectedRoot_,this,handle_,0,d))return false;
    sourcePC_=0x157ABC;if(!parseText157500(d))return false;
    source_->field428=0;sourceOpenReturned_=true;sourcePC_=0x157ACC;return true;
}
bool OriginalParticleSourceConfig50LA::abortNative(D& d)noexcept {
    d={};if(thread_!=std::this_thread::get_id()||commonMode2DrawAdapter_)return false;if(nativeClosed_)return true;
    //emitterForcedSets_ is a same-emitter identity borrow, not Config-owned
    //storage. Preserve its partial/publication history and allocation here.
    //Mode2's authentic RNG/arithmetic history and process-owner pin also
    //survive native cancellation; never restore, clear or reseed the stream.
    if(borrowed_){
        if(!resolver_||!resolver_->freshFileCapacity_||
           resolver_->freshFileCapacity_->particleConfigBorrower!=this)return fail(d,E::phase,0);
        auto& s=*resolver_->freshFileCapacity_;
        //Native withdrawal is not source1573B0/1574D8 or a source handle store.
        //A partially opened handle belongs to this exact address even when
        //open did not return to publish Config.file00.
        if(handle_>=0){
            if(s.members150&&s.devices14){
                if(!resolver_->closeSourceBankMember131040(selectedRoot_,this,handle_,false,d))return false;
            }else {
                auto* rows=s.members150?s.members150:s.failedCourseMembers;
                const auto count=s.members150?s.memberCount154:s.failedCourseMemberCapacity;
                if(!rows||handle_>=count)return fail(d,E::phase,0);
                auto& row=rows[handle_];
                if(row.courseConfigOwner!=this||!(row.flags18&4)||row.associatedObject1C||!row.originalMember)
                    return fail(d,E::phase,0);
                if(row.auxiliarySlot04>=0&&std::size_t(row.auxiliarySlot04)<resolver_->scratch_->slots.size()){
                    auto& slot=resolver_->scratch_->slots[row.auxiliarySlot04];
                    if(slot.status08==handle_)resolver_->scratch_->selected148=row.auxiliarySlot04;
                }
                row.flags18&=~std::uint32_t(4);row.courseConfigOwner=nullptr;
            }
        }
        s.particleConfigBorrower=nullptr;borrowed_=false;
    }
    OriginalConfigProcessGlobals50LA::expireStackNative(&emitterMinRange30_);
    OriginalConfigProcessGlobals50LA::expireStackNative(&emitterMaxRange30_);
    OriginalConfigProcessGlobals50LA::expireStackNative(&commonVelocity30_);
    for(auto& input:type0Noise30_)OriginalConfigProcessGlobals50LA::expireStackNative(&input);
    for(auto& input:type2Range30_)OriginalConfigProcessGlobals50LA::expireStackNative(&input);
    for(auto& input:type9Range30_)OriginalConfigProcessGlobals50LA::expireStackNative(&input);
    for(auto& input:type11Range30_)OriginalConfigProcessGlobals50LA::expireStackNative(&input);
    for(auto& input:type12Range30_)OriginalConfigProcessGlobals50LA::expireStackNative(&input);
    handle_=-1;selectedRoot_=nullptr;resolver_.reset();provider_.reset();nativeClosed_=true;return true;
}

bool OriginalParticleSourceConfig50LA::seek(std::size_t at,D& d)noexcept {
    if(!current())return fail(d,E::phase,0x12F640);
    return resolver_->seekSourceBankMember12F640(selectedRoot_,this,handle_,at,d);
}

bool OriginalParticleSourceConfig50LA::missingOpenCurrent()const noexcept {
    if(!attempted_||nativeClosed_||sourceClosed_||!borrowed_||
       thread_!=std::this_thread::get_id()||!resolver_||!provider_||
       !resolver_->freshFileCapacity_||resolver_->freshFileCapacity_->particleConfigBorrower!=this||
       resolver_->freshFileCapacity_->provider!=provider_||!sourceOpenReturned_||*sourceOpenReturned_||
       sourcePC_!=0x157acc||handle_!=-1||selectedRoot_||textAttempted_||textReturned_)return false;
    const auto& s=*source_;
    return s.file00==-1&&!s.binary08&&!s.opened0C&&!s.mode10&&!s.count14&&!s.index18&&
        !s.text1C&&!s.child24&&!s.selected20&&!s.cacheCount438&&!s.names4CC&&!s.index4BC&&!s.block4C4;
}
bool OriginalParticleSourceConfig50LA::closeStack1573B0(D& d)noexcept {
    d={};if(occurrence_!=Occurrence::manager1617A0||emitterOwner_)return false;
    const bool missing=missingOpenCurrent();
    if(sourceDestructorAttempted_||(!missing&&(!current()||!textReturned_||!sourceOpenReturned_||
       !*sourceOpenReturned_||source_->binary08||!glowGetterReturned_||
       integerGetterBounded_||frameGetterBounded_||textSelectBounded_)))return false;
    return closeOwnedTextStack1573B0(missing,d);
}
bool OriginalParticleSourceConfig50LA::closeEmitterStack1573B0(const void* owner,D& d)noexcept {
    d={};
    if(occurrence_!=Occurrence::emitter15AE00||!owner||emitterOwner_!=owner||commonMode2DrawAdapter_||
       !emitterForcedSets_||emitterForcedSets_->emitter_!=owner||!emitterForcedSets_->returned15B5C4_||
       !emitterPropertiesReturnedNative()||
       !commonReturned_||sourceDestructorAttempted_||sourceClosed_||!current()||!textReturned_||
       !sourceOpenReturned_||!*sourceOpenReturned_||source_->binary08||textSelectBounded_)return false;
    return closeOwnedTextStack1573B0(false,d);
}
bool OriginalParticleSourceConfig50LA::closeOwnedTextStack1573B0(bool missing,D& d)noexcept {
    auto& s=*source_;
    //All native custody guards precede the actual handle close. This is the
    //opened TEXT occurrence or the genuine unopened missing-member branch.
    if(commonMode2DrawAdapter_||s.child24!=textChildTable_.get()||s.text1C!=textTable_.sourceStorage||
       s.count14!=textTable_.count14||s.index18!=textTable_.capacity18||
       s.cacheCount438||s.names4CC||s.index4BC||s.block4C4)return false;
    sourceDestructorAttempted_=true;
    if(!missing){
    sourcePC_=0x1573E8u;
    //Actual same numeric handle:12F290 archive flush13D9E0 is a literal no-op,
    //followed by131040 closing that occurrence, NOT its bank or sharedwindow.
    if(!resolver_->closeSourceBankMember131040(selectedRoot_,this,handle_,true,d))return false;
    sourcePC_=0x157404u;s.file00=-1;
    handle_=-1; //Native custody withdrawn only AFTER real close+sourceSD.
    } //Missing file00=-1 takes1573DC directly to15740C: no flush/close/store.
    sourcePC_=0x15740Cu;
    if(!retireTextChild157F90())return false;
    s.selected20=0;s.child24=nullptr; //157CE4/E8 even when childwasNULL.
    const auto cleanup=Text::cleanupText157414(textTable_);sourcePC_=cleanup.pc;
    syncTextFields();if(cleanup.stop!=Text::Stop::complete)return false;
    //This actual text occurrence never populated438 cache or binary arrays.
    //Do not invent releasecalls for them, nor reset source capacity18.
    s.cacheCount438=0;sourcePC_=0x157478u;
    //Actual161AAC/15AE54 a1=-1 is signed16 negative:1574C8 skips2E8B00. Config was
    //on source stack; native embedded Source remains for historical evidence.
    sourceClosed_=true;sourcePC_=0x1574F4u;return true;
}

//Immutable historical validation, separate from the live Owner/Application
//identity checks in the cold aggregate. Later genuine308030 consumers may
//advance the shared raw state; equality with that live raw would be wrong.
bool OriginalParticleSourceConfig50LA::commonMode2CurrentNative(
    const std::array<std::uint32_t,0x260/4>& words,
    const std::array<std::uint32_t,0x260/4>& known)const noexcept {
    const auto& h=commonMode2_;
    if(commonMode2DrawAdapter_)return false; //No callable crosses a public boundary.
    const unsigned mask=(h.minimum?1u:0u)|(h.converted?2u:0u)|(h.maximum?4u:0u)|
        (h.unit?8u:0u)|(h.difference?16u:0u)|(h.product?32u:0u)|
        (h.sum?64u:0u)|(h.stored?128u:0u);
    if(!h.pc)return !h.applicationPin&&!h.application&&!h.rawBefore&&!h.rawAfter&&!h.rawValue&&
        !h.drawEntered&&!h.drawReturned&&!mask&&!commonMode2TimeStored_;
    if(!commonEmitMode5C_||*commonEmitMode5C_!=2||commonTailScalarCount_!=4||
       commonModeScalarCount_!=4||commonModeTimeCopied_||
       (h.pc!=0x15B1D0u&&sourcePC_!=h.pc))return false;
    if(!h.drawEntered)return !h.applicationPin&&!h.application&&!h.rawBefore&&!h.rawAfter&&!h.rawValue&&
        !h.drawReturned&&!mask&&!commonMode2TimeStored_&&
        (h.pc==0x15B148u||h.pc==0x308030u||h.pc==0x308038u);
    if(!h.applicationPin||!h.application)return false;
    if(!h.drawReturned)return !h.rawAfter&&!h.rawValue&&!mask&&!commonMode2TimeStored_&&
        (h.pc==0x308044u||h.pc==0x30805Cu);
    if(h.rawAfter!=h.rawBefore*0x41c64e6du+0x3039u||h.rawValue!=(h.rawAfter&0x7fffffffu))return false;
    //Each refused read/arithmetic operation retains precisely its predecessor
    //receipts. A complete history freezes at the common15B1D0 join even after
    //Properties, Animation, forced sets, source destruction or native abort.
    unsigned expected=0;
    switch(h.pc){
    case 0x30805Cu:case 0x15B150u:case 0x15B154u:expected=0;break;
    case 0x15B158u:expected=1;break;
    case 0x15B160u:expected=3;break;
    case 0x15B16Cu:expected=7;break;
    case 0x15B170u:expected=15;break;
    case 0x15B174u:expected=31;break;
    case 0x15B178u:expected=63;break;
    case 0x15B17Cu:case 0x15B180u:expected=127;break;
    case 0x15B1D0u:expected=255;break;
    default:return false;
    }
    if(mask!=expected||commonMode2TimeStored_!=bool(h.stored))return false;
    if(h.minimum&&(known[0x1e8/4]!=0xffffffffu||words[0x1e8/4]!=*h.minimum))return false;
    if(h.maximum&&(known[0x1ec/4]!=0xffffffffu||words[0x1ec/4]!=*h.maximum))return false;
    std::uint32_t result;
    if(h.converted&&(!rr::game::detail::sourceParticleTruncateInteger50LA(h.rawValue,result)||result!=*h.converted))return false;
    if(h.unit&&(!rr::game::detail::sourceParticleMode2Scale(*h.converted,result)||result!=*h.unit))return false;
    if(h.difference&&(!rr::game::detail::sourceParticleMode2Difference(*h.maximum,*h.minimum,result)||result!=*h.difference))return false;
    if(h.product&&(!rr::game::detail::sourceParticleMode2Product(*h.difference,*h.unit,result)||result!=*h.product))return false;
    if(h.sum&&(!rr::game::detail::sourceParticleMode2Sum(*h.minimum,*h.product,result)||result!=*h.sum))return false;
    if(h.stored&&(*h.stored!=*h.sum||known[0x1e4/4]!=0xffffffffu||words[0x1e4/4]!=*h.stored))return false;
    return true;
}

//Live file-custody validation for the separately-owned emitter stack Config.
//Native cancellation retains source bytes/history, but owns no file or bank.
bool OriginalParticleSourceConfig50LA::emitterOccurrenceCurrentNative(const void* owner,
    const OriginalStandardTextureResolver50LA* resolver,const OriginalStandardArchiveProvider50LA* provider,
    const OriginalParticleForcedSets50LA& forcedSets,const std::array<std::uint32_t,0x260/4>& words,
    const std::array<std::uint32_t,0x260/4>& known)const noexcept {
    if(occurrence_!=Occurrence::emitter15AE00||!owner||emitterOwner_!=owner||
       thread_!=std::this_thread::get_id()||source_!=&storage_||emitterForcedSets_!=&forcedSets||
       !forcedSets.currentNative(owner,words,known)||!commonMode2CurrentNative(words,known))return false;
    if(emitterPropertiesAttempted_!=bool(emitterPropertyTarget_)||
       (emitterPropertyTarget_&&*emitterPropertyTarget_!=0x15c330u&&*emitterPropertyTarget_!=0x15d560u&&*emitterPropertyTarget_!=0x15dd00u&&*emitterPropertyTarget_!=0x15e6d0u&&*emitterPropertyTarget_!=0x15ef70u&&*emitterPropertyTarget_!=0x15fc10u&&*emitterPropertyTarget_!=0x1602b0u))return false;
    if((emitterPropertyTarget_==0x15c330u||emitterPropertyTarget_==0x15d560u||emitterPropertyTarget_==0x15dd00u||emitterPropertyTarget_==0x15ef70u||emitterPropertyTarget_==0x15fc10u||emitterPropertyTarget_==0x1602b0u)&&(emitterGeneralReturned_||emitterGeneralFound_||
       emitterMinRangeFound_||emitterMaxRangeFound_||emitterMinRangeKnownBytes_||emitterMaxRangeKnownBytes_||
       emitterVectorWritten_[0]||emitterVectorWritten_[1]||emitterMinRangeReturned_||emitterMaxRangeReturned_||
       emitterSpreadReturned_||emitterType8Returned_))return false;
    if(type0GeneralSelectionAttempted_&&(emitterPropertyTarget_!=0x15c330u||!commonReturned_))return false;
    if(type0GeneralReturned_!=bool(type0GeneralFound_)||
       (type0GeneralReturned_&&(!type0GeneralSelectionAttempted_||!textSelectionReturned_||textSelectBounded_)))return false;
    if(type0ScalarCount_>14||type0AssertionCount_>type0ScalarCount_||
       type0ScalarCount_>type0AssertionCount_+1||
       (type0ScalarCount_&&!type0GeneralReturned_))return false;
    if(type0AssertionCount_&&!OriginalConfigProcessGlobals50LA::type0AssertionsCurrentNative(type0AssertionCount_))return false;
    if(type0NoiseReturned_>2||(type0NoiseReturned_&&type0AssertionCount_!=14))return false;
    for(std::size_t i=0;i<2;++i){
        if(type0NoiseFound_[i]&&(type0AssertionCount_!=14||(i&&type0NoiseReturned_<1)))return false;
        if(type0NoiseKnownBytes_[i]&&(!type0NoiseFound_[i]||
           type0NoiseKnownBytes_[i]!=(*type0NoiseFound_[i]?128u:1u)))return false;
        const auto mask=type0NoiseWritten_[i];if(mask!=0&&mask!=1&&mask!=3&&mask!=7)return false;
        if((mask&&!type0NoiseFound_[i])||(type0NoiseReturned_>i&&mask!=7))return false;
    }
    if(type2GeneralReturned_!=bool(type2GeneralFound_)||
       (type2GeneralReturned_&&(emitterPropertyTarget_!=0x15dd00u||!emitterPropertiesAttempted_||
           (!commonPropertiesAttempted_&&(!textSelectionReturned_||textSelectBounded_)))))return false;
    if(type2MovingFound_&&!type2GeneralReturned_)return false;
    if(type2MovingReturned_&&!type2MovingFound_)return false;
    if(type2RangeReturned_>2||(type2RangeReturned_&&!type2MovingReturned_))return false;
    for(std::size_t i=0;i<2;++i){
        if(type2RangeFound_[i]&&(!type2MovingReturned_||(i&&type2RangeReturned_<1)))return false;
        if(type2RangeKnownBytes_[i]&&(!type2RangeFound_[i]||
           type2RangeKnownBytes_[i]!=(*type2RangeFound_[i]?128u:1u)))return false;
        const auto mask=type2RangeWritten_[i];if(mask!=0&&mask!=1&&mask!=3&&mask!=7)return false;
        if((mask&&!type2RangeFound_[i])||(type2RangeReturned_>i&&mask!=7))return false;
    }
    if(type9GeneralReturned_!=bool(type9GeneralFound_)||
       (type9GeneralReturned_&&(emitterPropertyTarget_!=0x15ef70u||!emitterPropertiesAttempted_||
           (!commonPropertiesAttempted_&&(!textSelectionReturned_||textSelectBounded_)))))return false;
    if(type9RangeReturned_>2||(type9RangeReturned_&&!type9GeneralReturned_))return false;
    for(std::size_t i=0;i<2;++i){
        if(type9RangeFound_[i]&&(!type9GeneralReturned_||(i&&type9RangeReturned_<1)))return false;
        if(type9RangeKnownBytes_[i]&&(!type9RangeFound_[i]||
           type9RangeKnownBytes_[i]!=(*type9RangeFound_[i]?128u:1u)))return false;
        const auto mask=type9RangeWritten_[i];if(mask!=0&&mask!=1&&mask!=3&&mask!=7)return false;
        if((mask&&!type9RangeFound_[i])||(type9RangeReturned_>i&&mask!=7))return false;
    }
    if(type9SpreadFound_&&type9RangeReturned_!=2)return false;
    if(type9SpreadReturned_&&!type9SpreadFound_)return false;
    if(type11GeneralReturned_!=bool(type11GeneralFound_)||
       (type11GeneralReturned_&&(emitterPropertyTarget_!=0x15fc10u||!emitterPropertiesAttempted_||
           (!commonPropertiesAttempted_&&(!textSelectionReturned_||textSelectBounded_)))))return false;
    if(type11ScalarCount_>6||(type11ScalarCount_&&!type11GeneralReturned_))return false;
    if(type11HeadFound_&&type11ScalarCount_!=6)return false;
    if(type11HeadReturned_&&!type11HeadFound_)return false;
    if(type11RangeReturned_>2||(type11RangeReturned_&&!type11HeadReturned_))return false;
    for(std::size_t i=0;i<2;++i){
        if(type11RangeFound_[i]&&(!type11HeadReturned_||(i&&type11RangeReturned_<1)))return false;
        if(type11RangeKnownBytes_[i]&&(!type11RangeFound_[i]||
           type11RangeKnownBytes_[i]!=(*type11RangeFound_[i]?128u:1u)))return false;
        const auto mask=type11RangeWritten_[i];if(mask!=0&&mask!=1&&mask!=3&&mask!=7)return false;
        if((mask&&!type11RangeFound_[i])||(type11RangeReturned_>i&&mask!=7))return false;
    }
    if(type11MovingFound_&&type11RangeReturned_!=2)return false;
    if(type11MovingReturned_&&!type11MovingFound_)return false;
    if(type12GeneralReturned_!=bool(type12GeneralFound_)||
       (type12GeneralReturned_&&(emitterPropertyTarget_!=0x1602b0u||!emitterPropertiesAttempted_||
           (!commonPropertiesAttempted_&&(!textSelectionReturned_||textSelectBounded_)))))return false;
    if(type12RangeReturned_>2||(type12RangeReturned_&&!type12GeneralReturned_))return false;
    for(std::size_t i=0;i<2;++i){
        if(type12RangeFound_[i]&&(!type12GeneralReturned_||(i&&type12RangeReturned_<1)))return false;
        if(type12RangeKnownBytes_[i]&&(!type12RangeFound_[i]||
           type12RangeKnownBytes_[i]!=(*type12RangeFound_[i]?128u:1u)))return false;
        const auto mask=type12RangeWritten_[i];if(mask!=0&&mask!=1&&mask!=3&&mask!=7)return false;
        if((mask&&!type12RangeFound_[i])||(type12RangeReturned_>i&&mask!=7))return false;
    }
    if(type12DistanceInput_&&type12RangeReturned_!=2)return false;
    if(type12SquaredBits_&&!type12DistanceInput_)return false;
    if(type12DistanceSquared_!=bool(type12SquaredBits_))return false;
    if(type12ReciprocalBits_&&!type12DistanceSquared_)return false;
    if(type12ReciprocalWritten_!=bool(type12ReciprocalBits_))return false;
    if(type12TailScalarCount_>3||(type12TailScalarCount_&&!type12ReciprocalWritten_))return false;
    if(type12OscillationEnabled_&&type12TailScalarCount_!=3)return false;
    if(type12OscillationMask_!=0&&type12OscillationMask_!=1&&type12OscillationMask_!=3&&type12OscillationMask_!=7)return false;
    if(type12OscillationMask_&&type12OscillationEnabled_!=true)return false;
    if(type12FlagMask_!=0&&type12FlagMask_!=1&&type12FlagMask_!=3)return false;
    if(type12FlagMask_&&(!commonReturned_||emitterPropertyTarget_!=0x1602b0u))return false;
    if(emitterPropertiesAttempted_&&(!sourceOpenReturned_||!*sourceOpenReturned_||!textReturned_))return false;
    if(emitterGeneralReturned_!=bool(emitterGeneralFound_)||
       (emitterGeneralReturned_&&(!emitterPropertiesAttempted_||
           (!commonPropertiesAttempted_&&(!textSelectionReturned_||textSelectBounded_)))))return false;
    if(emitterMinRangeFound_&&!emitterGeneralReturned_)return false;
    if(emitterMinRangeKnownBytes_&&(!emitterMinRangeFound_||
       emitterMinRangeKnownBytes_!=(*emitterMinRangeFound_?128u:1u)))return false;
    if(emitterMaxRangeFound_&&!emitterMinRangeReturned_)return false;
    if(emitterMaxRangeKnownBytes_&&(!emitterMaxRangeFound_||
       emitterMaxRangeKnownBytes_!=(*emitterMaxRangeFound_?128u:1u)))return false;
    for(std::size_t i=0;i<2;++i){
        const auto mask=emitterVectorWritten_[i];if(mask!=0&&mask!=1&&mask!=3&&mask!=7)return false;
        if(mask&&!(i?emitterMaxRangeFound_:emitterMinRangeFound_))return false;
    }
    if((emitterMinRangeReturned_&&emitterVectorWritten_[0]!=7)||
       (emitterMaxRangeReturned_&&(!emitterMinRangeReturned_||emitterVectorWritten_[1]!=7))||
       (emitterSpreadReturned_&&!emitterMaxRangeReturned_))return false;
    if(commonPropertiesAttempted_&&(emitterPropertyTarget_==0x15e6d0u?!emitterSpreadReturned_:
        (emitterPropertyTarget_==0x15dd00u?type2RangeReturned_!=2:(emitterPropertyTarget_==0x15ef70u?!type9SpreadReturned_:(emitterPropertyTarget_==0x15fc10u?!type11MovingReturned_:(emitterPropertyTarget_==0x1602b0u?(!type12OscillationEnabled_||type12TailScalarCount_!=3||type12OscillationMask_!=(*type12OscillationEnabled_?7u:0u)):(emitterPropertyTarget_!=0x15c330u&&emitterPropertyTarget_!=0x15d560u)))))))return false;
    if(commonGeneralReturned_!=bool(commonGeneralFound_)||
       (commonGeneralReturned_&&(!commonPropertiesAttempted_||
           (!commonPropertiesSelectionAttempted_&&(!textSelectionReturned_||textSelectBounded_)))))return false;
    if(commonGroupFound_&&!commonGeneralReturned_)return false;
    if(commonGroupWritten_&&!commonGroupFound_)return false;
    if(commonGroupAssertionReturned_&&!commonGroupWritten_)return false;
    if(commonScalarCount_>5||(commonScalarCount_&&!commonGroupAssertionReturned_))return false;
    if(commonAgeScaled_&&commonScalarCount_!=5)return false;
    if(commonVelocityFound_&&!commonAgeScaled_)return false;
    if(commonVelocityKnownBytes_&&(!commonVelocityFound_||
       commonVelocityKnownBytes_!=(*commonVelocityFound_?128u:1u)))return false;
    if(commonVelocityWritten_!=0&&commonVelocityWritten_!=1&&commonVelocityWritten_!=3&&commonVelocityWritten_!=7)return false;
    if((commonVelocityWritten_&&!commonVelocityFound_)||(commonVelocityReturned_&&commonVelocityWritten_!=7))return false;
    if(commonTailAttempted_&&!commonVelocityReturned_)return false;
    if(commonEmitMode5C_&&!commonTailAttempted_)return false;
    if(commonModeStored_&&!commonEmitMode5C_)return false;
    if(commonTailScalarCount_>4||(commonTailScalarCount_&&!commonModeStored_))return false;
    if(commonDelayCopied_&&commonTailScalarCount_<1)return false;
    if(commonModeScalarCount_>4||(commonModeScalarCount_&&commonTailScalarCount_!=4))return false;
    if(commonModeTimeCopied_&&(!commonEmitMode5C_||
       (*commonEmitMode5C_==1?commonModeScalarCount_!=1:*commonEmitMode5C_==3?commonModeScalarCount_!=2:true)))return false;
    if(commonPropertiesSelectionAttempted_&&(commonTailScalarCount_!=4||!commonEmitMode5C_))return false;
    if(commonPropertiesSelectionAttempted_&&(*commonEmitMode5C_==1||*commonEmitMode5C_==3)&&!commonModeTimeCopied_)return false;
    if(commonPropertiesSelectionAttempted_&&*commonEmitMode5C_==2&&!commonMode2TimeStored_)return false;
    if(commonPropertiesSelected_!=bool(commonPropertiesFound_)||
       (commonPropertiesSelected_&&(!commonPropertiesSelectionAttempted_||
        (!commonAnimationSelectionAttempted_&&(!textSelectionReturned_||textSelectBounded_)))))return false;
    if(commonPropertyCount_>8||(commonPropertyCount_&&!commonPropertiesSelected_))return false;
    if(commonMirrorCount_>2||(commonMirrorCount_&&commonPropertyCount_!=8))return false;
    if(commonFlagCount_>8||(commonFlagCount_&&commonMirrorCount_!=2))return false;
    if(commonGroundFlagWritten_&&commonFlagCount_!=8)return false;
    if(commonAnimationSelectionAttempted_&&!commonGroundFlagWritten_)return false;
    if(commonAnimationSelected_!=bool(commonAnimationFound_)||
       (commonAnimationSelected_&&(!commonAnimationSelectionAttempted_||
           (!type0GeneralSelectionAttempted_&&(!textSelectionReturned_||textSelectBounded_)))))return false;
    if(commonFramesWritten_&&!commonAnimationSelected_)return false;
    if(commonReciprocalWritten_&&!commonFramesWritten_)return false;
    if(commonForcedCountWritten_&&!commonReciprocalWritten_)return false;
    if(commonAnimationFlagWritten_&&!commonForcedCountWritten_)return false;
    if(commonReturned_&&!commonAnimationFlagWritten_)return false;
    if(forcedSets.countRead15B508_&&!commonAnimationFlagWritten_)return false;
    if(commonReturned_!=forcedSets.returned15B5C4_)return false;
    if(emitterType8Returned_&&(emitterPropertyTarget_!=0x15e6d0u||!commonReturned_))return false;
    if(emitterType0Returned_&&(emitterPropertyTarget_!=0x15c330u||type0NoiseReturned_!=2||!commonReturned_))return false;
    if(emitterType1Returned_&&(emitterPropertyTarget_!=0x15d560u||!commonReturned_))return false;
    if(emitterType2Returned_&&(emitterPropertyTarget_!=0x15dd00u||type2RangeReturned_!=2||!commonReturned_))return false;
    if(emitterType9Returned_&&(emitterPropertyTarget_!=0x15ef70u||!type9SpreadReturned_||!commonReturned_))return false;
    if(emitterType11Returned_&&(emitterPropertyTarget_!=0x15fc10u||!type11MovingReturned_||!commonReturned_))return false;
    if(emitterType12Returned_&&(emitterPropertyTarget_!=0x1602b0u||type12FlagMask_!=3||!commonReturned_))return false;
    if(sourceDestructorAttempted_&&!emitterPropertiesReturnedNative())return false;
    if(sourceClosed_&&!sourceDestructorAttempted_)return false;
    if(nativeClosed_)return !borrowed_&&!resolver_&&!provider_&&handle_==-1&&!selectedRoot_;
    if(!attempted_)return !borrowed_&&!resolver_&&!provider_&&handle_==-1&&!selectedRoot_&&!sourceOpenReturned_;
    if(!borrowed_||!resolver_||resolver_.get()!=resolver||!provider_||provider_.get()!=provider||
       !resolver_->freshFileCapacity_||resolver_->freshFileCapacity_->particleConfigBorrower!=this||
       resolver_->freshFileCapacity_->provider!=provider_)return false;
    if(sourceDestructorAttempted_){
        //A genuine source close may have closed the numeric member before a
        //later text cleanup bound. Preserve that partial state for nativeabort.
        if(!sourceOpenReturned_||!*sourceOpenReturned_||!textReturned_||source_->binary08)return false;
        if(handle_>=0)return !sourceClosed_&&source_->file00==handle_&&
            resolver_->sourceBankMemberCurrent(selectedRoot_,this,handle_);
        if(source_->file00!=-1)return false;
        if(sourceClosed_)return !source_->child24&&!source_->selected20&&!source_->text1C&&
            source_->count14==textTable_.count14&&textTable_.sourceCleaned&&
            !textChildTable_&&!textTable_.sourceStorage;
        return source_->child24==textChildTable_.get()&&source_->text1C==textTable_.sourceStorage;
    }
    if(sourceOpenReturned_)return *sourceOpenReturned_?(current()&&textReturned_&&!source_->binary08):missingOpenCurrent();
    if(handle_>=0){
        const auto& s=*resolver_->freshFileCapacity_;
        const auto* rows=s.members150?s.members150:s.failedCourseMembers;
        const auto count=s.members150?s.memberCount154:s.failedCourseMemberCapacity;
        if(!rows||handle_>=count||rows[handle_].courseConfigOwner!=this||
           !(rows[handle_].flags18&4)||rows[handle_].associatedObject1C||!rows[handle_].originalMember)return false;
    }
    return true; //A real partial open/read/parser may not yet publish file00.
}

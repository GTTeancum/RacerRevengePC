//Distinct native-adapted occurrence2B9D04. Source Config/heap frontiers stay
//unjoined. Read the SAME Application directory at first strlen, not a previous
//provider path snapshot. Each nonempty%s flushes before the next argument.
bool Owner::preflightNativeWaterPoolPathFormat()const noexcept {
    if(!preflightNativeWaterPoolAllocationEntry())return false;
    using N=SourceWaterConfig::NativeWaterPoolPathFormat;using Phase=N::Phase;
    using G=a::OriginalConfigProcessGlobals50LA;
    const auto& n=sourceWaterConfig_.nativeWaterPoolPathFormat;
    const auto virginFrame=[](const N::Frame& f)noexcept {return !f.entered&&!f.released&&!f.saves&&!f.restores;};
    const auto openFrame=[](const N::Frame& f,std::uint16_t mask)noexcept {return f.entered&&!f.released&&f.saves==mask&&!f.restores;};
    const auto closedFrame=[](const N::Frame& f,std::uint16_t mask)noexcept {return f.entered&&f.released&&f.saves==mask&&f.restores==mask;};
    const auto virginString=[&](const N::String& s)noexcept {return !s.consumed&&!s.strlenIssued&&!s.lengthProjected&&!s.enqueued&&
        !s.completed&&!s.copied&&!s.length&&!s.readWindowMax&&!s.known&&!s.directoryPublication&&
        virginFrame(s.flush)&&virginFrame(s.writer)&&!s.writerReturn&&!s.flushReturn;};
    const auto noStrings=[&]()noexcept {return virginString(n.strings[0])&&virginString(n.strings[1]);};
    const auto noQueue=[&]()noexcept {return !n.queueInitialized&&!n.formatterCount&&!n.uioCount&&!n.uioResidual;};
    const auto noFinish=[&]()noexcept {return !n.formatterReturn&&!n.wrapperReturn&&!n.terminated&&!n.raRestored&&!n.wrapperReleased&&
        !n.stdio.restores&&!n.core.restores&&!n.stdio.released&&!n.core.released;};
    const auto noFile=[&]()noexcept {return !n.capturedReent&&!n.fileReent54&&!n.flags0C&&!n.buffer10&&!n.bufferSize14&&
        !n.remaining08&&!n.cursor00&&!n.savedDirectoryIdentity&&!n.savedFilenameIdentity&&!n.varargsSaveMask&&!n.raSaved;};
    if(n.scanCount>3u||n.completed>2u||n.copied>255u||n.known!=n.copied+(n.terminated?1u:0u))return false;
    for(std::size_t i=n.scanCount;i<n.scans.size();++i)
        if(n.scans[i].offset||n.scans[i].returned||n.scans[i].length||n.scans[i].wide)return false;
    if(!n.attempted)return n.phase==Phase::unentered&&!n.pc&&!n.callIssued&&!n.wrapperFrame&&!n.directoryOwnerIdentity&&
        noFile()&&!n.initializerReturned&&!n.localeReturned&&virginFrame(n.stdio)&&virginFrame(n.core)&&
        !n.scanCount&&!n.completed&&!n.copied&&!n.known&&noQueue()&&noStrings()&&noFinish();
    const auto& c=sourceWaterConfigOwner_;
    if(!c||c->pc_!=0x30492cu||c->wakeSelection_.returned||!c->nativeArrayScalars_.entered||
       c->nativeArrayScalars_.signature||c->nativeArrayScalars_.completed!=3u||c->nativeArrayScalars_.pc!=0x2b9cecu||
       sourceWaterConfig_.nativeWaterArrayFormat.role!=NativeWaterArrayRole::poolPointers||
       !n.callIssued||!n.wrapperFrame||!n.directoryOwnerIdentity||
       (!sourceWaterProvider274720_.nativeWithdrawn&&(!worldApplication_||n.directoryOwnerIdentity!=worldApplication_.get())))return false;
    if(!n.capturedReent)return n.phase==Phase::reent&&n.pc==0x308c98u&&noFile()&&
        !n.initializerReturned&&!n.localeReturned&&virginFrame(n.stdio)&&virginFrame(n.core)&&
        !n.scanCount&&!n.completed&&!n.copied&&noQueue()&&noStrings()&&noFinish();
    if(n.capturedReent!=G::StdioIdentity::reent385160||n.fileReent54!=n.capturedReent||n.flags0C!=0x208u||!n.buffer10||
       n.bufferSize14!=0x7fffffffu||n.remaining08!=0x7fffffffu-n.copied||n.cursor00!=n.copied||
       !n.savedDirectoryIdentity||n.savedFilenameIdentity!=&sourceWaterConfig_.arrayFilename790||n.varargsSaveMask!=63u||!n.raSaved)return false;
    if(!sourceWaterProvider274720_.nativeWithdrawn&&
       n.savedDirectoryIdentity!=&worldApplication_->sourceWaterProcessPaths_.directory36BFD0)return false;
    if(n.phase==Phase::stdio)return openFrame(n.stdio,15u)&&!n.initializerReturned&&!n.localeReturned&&virginFrame(n.core)&&
        !n.scanCount&&!n.completed&&!n.copied&&noQueue()&&noStrings()&&noFinish()&&
        (n.pc==0x30b50cu||n.pc==0x30b528u||n.pc==0x30b534u);
    if(!n.initializerReturned||n.stdio.saves!=15u||!n.stdio.entered||n.core.saves!=1023u||!n.core.entered)return false;
    if(n.phase==Phase::locale)return openFrame(n.stdio,15u)&&openFrame(n.core,1023u)&&!n.localeReturned&&
        !n.scanCount&&!n.completed&&!n.copied&&noQueue()&&noStrings()&&noFinish()&&
        (n.pc==0x305330u||n.pc==0x3052f0u||n.pc==0x30b5a8u);
    if(!n.localeReturned||!n.queueInitialized||!n.formatterCount||!n.uioCount||!n.uioResidual||!n.scanCount)return false;
    bool scanFailure=false;
    for(std::size_t i=0;i<n.scanCount;++i){
        const auto& s=n.scans[i];if(s.offset!=2u*i)return false;
        if(!s.returned){
            if(i+1u!=n.scanCount||s.length||s.wide||(n.pc!=0x30b658u&&n.pc!=0x30b65cu))return false;scanFailure=true;
        }else if(s.length==-1){
            if(i+1u!=n.scanCount||s.wide||n.pc!=0x30b674u)return false;scanFailure=true;
        }else if(s.length!=(i<2u?1:0)||s.wide!=(i<2u?std::uint32_t('%'):0u))return false;
    }
    std::size_t completed=0,copied=0,total=0;std::uint32_t pendingCount=0,pendingBytes=0;
    for(std::size_t i=0;i<2u;++i){
        const auto& s=n.strings[i];
        if(!s.consumed){if(!virginString(s))return false;continue;}
        if(i!=completed||n.scanCount<i+1u||!n.scans[i].returned||n.scans[i].length!=1||!s.strlenIssued)return false;
        if(!s.lengthProjected){
            if(s.length||s.readWindowMax||s.known||s.directoryPublication||s.enqueued||s.completed||s.copied||
               !virginFrame(s.flush)||!virginFrame(s.writer)||s.writerReturn||s.flushReturn||
               n.phase!=Phase::argument||n.pc!=0x3093a8u||n.scanCount!=i+1u||scanFailure)return false;
            continue;
        }
        if(!s.length||*s.length>(i?127u:255u)||s.known!=*s.length+1u||
           s.readWindowMax!=((*s.length+16u)&~std::size_t(15u))||!s.enqueued||
           (i?bool(s.directoryPublication):!s.directoryPublication))return false;
        for(std::size_t j=0;j<*s.length;++j)
            if(s.bytes[j]==0u)return false;
        if(s.bytes[*s.length]!=0)return false;
        //The directory snapshot is a historical read. Later represented writes
        //may advance its publication; invalidation does not rewrite old history.
        if(!i&&!sourceWaterProvider274720_.nativeWithdrawn){
            const auto& paths=worldApplication_->sourceWaterProcessPaths_;
            if(paths.publication<*s.directoryPublication)return false;
            if(!paths.unknown&&paths.publication==*s.directoryPublication)
                for(std::size_t j=0;j<s.known;++j)if(static_cast<unsigned char>(paths.directory36BFD0[j])!=s.bytes[j])return false;
        }
        if(i){const auto& bytes=sourceWaterConfig_.arrayFilename790;
            for(std::size_t j=0;j<s.known;++j)
                if(!bytes.known[j]||bytes.value[j]!=s.bytes[j])return false;
        }
        total+=*s.length;
        if(*s.length){
            if(s.completed){
                if(!s.copied||!closedFrame(s.flush,3u)||!closedFrame(s.writer,511u)||s.writerReturn!=0u||s.flushReturn!=0u)return false;
            }else{
                if(s.copied||!openFrame(s.flush,3u)||!openFrame(s.writer,511u)||s.writerReturn||s.flushReturn||
                   n.phase!=Phase::copy||n.pc!=0x304f40u||n.scanCount!=i+1u||scanFailure)return false;
                pendingCount=1u;pendingBytes=static_cast<std::uint32_t>(*s.length);
            }
        }else if(!s.completed||s.copied||!virginFrame(s.flush)||!virginFrame(s.writer)||s.writerReturn||s.flushReturn)return false;
        if(s.copied){
            if(*s.length>255u-copied)return false;
            for(std::size_t j=0;j<*s.length;++j)if(n.output940[copied+j]!=s.bytes[j])return false;
            copied+=*s.length;
        }
        if(s.completed)++completed;
    }
    if(n.completed!=completed||n.copied!=copied||n.formatterCount!=total||n.uioCount!=pendingCount||n.uioResidual!=pendingBytes)return false;
    if(n.phase==Phase::scan)return scanFailure&&n.scanCount==completed+1u&&noFinish()&&
        openFrame(n.stdio,15u)&&openFrame(n.core,1023u);
    if(n.phase==Phase::argument||n.phase==Phase::copy)return completed<2u&&!scanFailure&&n.scanCount==completed+1u&&
        n.strings[completed].consumed&&noFinish()&&openFrame(n.stdio,15u)&&openFrame(n.core,1023u);
    return n.phase==Phase::callerBoundary&&n.pc==0x2b9d0cu&&completed==2u&&n.scanCount==3u&&!scanFailure&&
        n.terminated&&n.output940[n.copied]==0&&n.formatterReturn==n.copied&&n.wrapperReturn==n.formatterReturn&&
        n.raRestored&&n.wrapperReleased&&closedFrame(n.stdio,15u)&&closedFrame(n.core,1023u);
}
Owner::WaterPoolPathFormatResult Owner::formatWaterPoolPathNativeOnly(WaterArrayBackingMode mode)noexcept {
    WaterPoolPathFormatResult result;
    const auto& h=sourceWaterConfig_;const auto& r=sourceWaterProviderInitializer_;
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||closed_||
       nativeScbThread_!=std::this_thread::get_id()||runtimeCameraBorrows_||runtimeWorldBorrows_||
       constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||
       !h.wakeSelectionEntered||h.nativeWithdrawn||sourceWaterProvider274720_.nativeWithdrawn||
       !sourceWaterConfigOwner_||!sourceWaterProvider_||!worldApplication_||!sourceLoadApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||
       r.pc!=0x30492cu||sourcePostParticleModelsPC_!=r.pc||sourceWaterProvider274720_.pc!=r.pc||
       sourceWaterProviderNumeric_.pc!=r.pc||!preflightSourceWaterConfigNative())return result;
    const auto& scalar=sourceWaterConfigOwner_->nativeArrayScalars_;
    if(!scalar.entered||scalar.signature||scalar.completed!=3u||scalar.pc!=0x2b9cecu||
       h.nativeWaterArrayFormat.role!=NativeWaterArrayRole::poolPointers)return result;
    auto& n=sourceWaterConfig_.nativeWaterPoolPathFormat;
    result.originalSourceStop=sourceWaterConfigOwner_->pc_;result.selectionStop=sourceWaterConfigOwner_->nativeArraySelection_.pc;
    result.scalarStop=scalar.pc;result.allocatorStop=h.nativeWaterAllocationEntry.pc;
    const auto finish=[&](WaterPoolPathFormatResult::Status status)noexcept {
        result.status=status;result.adaptedPC=n.pc;result.formatterReturn=n.formatterReturn;
        result.completedStrings=n.completed;result.knownOutputBytes=n.known;return result;
    };
    if(n.attempted)return finish(WaterPoolPathFormatResult::Status::alreadyAttempted);
    using N=SourceWaterConfig::NativeWaterPoolPathFormat;using Phase=N::Phase;using G=a::OriginalConfigProcessGlobals50LA;
    const G::WaterSprintfAccess access;
    n.attempted=true;n.phase=Phase::reent;
    n.pc=0x2b9cec;n.pc=0x2b9cf0; //Exact immutable%s%s at3C0C28.
    n.pc=0x2b9cf4;n.pc=0x2b9cf8; //Unused t0=s0+4 is not consumed by this format.
    n.pc=0x2b9cfc;n.pc=0x2b9d00;n.directoryOwnerIdentity=worldApplication_.get(); //Pointer setup, no string read.
    n.pc=0x2b9d04;n.callIssued=true;n.pc=0x2b9d08;
    n.pc=0x308c90;n.pc=0x308c94;n.wrapperFrame=true; //224-byte frame.
    G::StdioIdentity captured;
    if(!G::readWaterReent308C98(access,captured,n.pc))return finish(WaterPoolPathFormatResult::Status::nativeAdmissionStopped);
    n.capturedReent=captured;n.pc=0x308c9c;n.pc=0x308ca0;n.pc=0x308ca4;
    n.pc=0x308ca8;n.savedDirectoryIdentity=&worldApplication_->sourceWaterProcessPaths_.directory36BFD0;n.varargsSaveMask|=1u;
    n.pc=0x308cac;n.pc=0x308cb0;n.raSaved=true;n.pc=0x308cb4;
    n.pc=0x308cb8;n.savedFilenameIdentity=&sourceWaterConfig_.arrayFilename790;n.varargsSaveMask|=2u;
    n.pc=0x308cbc;n.pc=0x308cc0;n.varargsSaveMask|=4u;n.pc=0x308cc4;n.varargsSaveMask|=8u;
    n.pc=0x308cc8;n.varargsSaveMask|=16u;n.pc=0x308ccc;n.varargsSaveMask|=32u; //Unused argument values remain unknown.
    n.pc=0x308cd0;n.flags0C=0x208u;n.pc=0x308cd4;n.buffer10=true;
    n.pc=0x308cd8;n.bufferSize14=0x7fffffffu;n.pc=0x308cdc;n.fileReent54=captured;
    n.pc=0x308ce0;n.cursor00=0u;n.pc=0x308ce4;n.pc=0x308ce8;n.remaining08=0x7fffffffu;
    n.phase=Phase::stdio;n.pc=0x30b4f0;n.stdio.entered=true; //64-byte frame, four actual saves.
    n.pc=0x30b4f4;n.stdio.saves|=2u;n.pc=0x30b4f8;n.stdio.saves|=4u;
    n.pc=0x30b4fc;n.pc=0x30b500;n.stdio.saves|=8u;n.pc=0x30b504;n.stdio.saves|=1u;n.pc=0x30b508;
    if(!G::initializeWaterSprintf30B528(access,*n.fileReent54,n.pc))return finish(WaterPoolPathFormatResult::Status::nativeAdmissionStopped);
    n.initializerReturned=true;n.phase=Phase::locale;n.pc=0x30b568;n.core.entered=true; //720-byte frame.
    n.pc=0x30b56c;n.core.saves|=32u;n.pc=0x30b570;n.core.saves|=512u;
    n.pc=0x30b574;n.pc=0x30b578;n.pc=0x30b57c;n.pc=0x30b580;n.core.saves|=1u;
    n.pc=0x30b584;n.core.saves|=2u;n.pc=0x30b588;n.core.saves|=4u;n.pc=0x30b58c;n.core.saves|=8u;
    n.pc=0x30b590;n.core.saves|=16u;n.pc=0x30b594;n.core.saves|=64u;n.pc=0x30b598;n.core.saves|=128u;
    n.pc=0x30b59c;n.core.saves|=256u;n.pc=0x30b5a0;n.pc=0x30b5a4;
    char unusedDecimal;
    if(!G::waterLocale305328(access,unusedDecimal,n.pc))return finish(WaterPoolPathFormatResult::Status::nativeAdmissionStopped);
    n.localeReturned=true;n.queueInitialized=true;n.formatterCount=0u;n.uioCount=0u;n.uioResidual=0u;
    const auto scan=[&](std::size_t ordinal)noexcept {
        n.phase=Phase::scan;auto& s=n.scans[n.scanCount++];s.offset=ordinal*2u;
        const unsigned char byte=ordinal<2u?'%':0u;std::uint32_t wide=0;std::int32_t length=0;
        if(!G::waterByte306108(access,byte,wide,length,n.pc))return false;
        s.returned=true;s.length=length;if(length>=0)s.wide=wide;
        if(length!=(byte?1:0)){n.pc=0x30b674;return false;}return true;
    };
    for(std::size_t ordinal=0;ordinal<2u;++ordinal){
        if(!scan(ordinal))return finish(WaterPoolPathFormatResult::Status::nativeAdmissionStopped);
        auto& s=n.strings[ordinal];n.phase=Phase::argument;n.pc=0x30b718; //Direct immutable 's' dispatch, not306108.
        n.pc=0x30bbd4;n.pc=0x30bbd8;s.consumed=true; //Saved pointer read only NOW, after any previous flush.
        n.pc=0x30bc18;n.pc=0x30bc1c;s.strlenIssued=true;n.pc=0x3093a8;
        const std::size_t capacity=ordinal?128u:256u;std::size_t length=0;
        const auto& paths=worldApplication_->sourceWaterProcessPaths_;
        if(!ordinal&&(worldApplication_->nativeOwnerThread_!=std::this_thread::get_id()||
           !worldApplication_->constructed_||worldApplication_->released_||paths.unknown))
            return finish(WaterPoolPathFormatResult::Status::nativeAdmissionStopped);
        //Admission inspects only this argument. No later argument receipt is
        //manufactured early. A known nonzero prefix+NUL projects strlen's result
        //without inventing aligned word values from unknown post-NUL bytes.
        for(;length<capacity;++length){
            if(ordinal&&!n.savedFilenameIdentity->known[length])break;
            const auto byte=ordinal?n.savedFilenameIdentity->value[length]:static_cast<unsigned char>(paths.directory36BFD0[length]);
            if(!byte)break;
        }
        if(length==capacity||(ordinal&&!n.savedFilenameIdentity->known[length]))
            return finish(WaterPoolPathFormatResult::Status::nativeAdmissionStopped);
        for(std::size_t i=0;i<=length;++i)s.bytes[i]=ordinal?n.savedFilenameIdentity->value[i]:static_cast<unsigned char>(paths.directory36BFD0[i]);
        s.known=length+1u;s.length=length;s.readWindowMax=(length+16u)&~std::size_t(15u);s.lengthProjected=true;
        if(!ordinal)s.directoryPublication=paths.publication; //Historical capture, no source global write.
        n.pc=0x3094dc;n.pc=0x30bc20;
        //No flags/precision/padding: enqueue exactly this string, even if empty.
        n.pc=0x30c1b4;n.pc=0x30c1b8;n.pc=0x30c1c0;n.pc=0x30c1c4;n.pc=0x30c1c8;n.pc=0x30c1cc;
        n.pc=0x30c8d8;n.pc=0x30c8dc;n.uioResidual=static_cast<std::uint32_t>(length);
        n.pc=0x30c8e4;n.uioCount=1u;s.enqueued=true;
        n.pc=0x30c9d0;n.pc=0x30c9f0;n.pc=0x30c9f4;*n.formatterCount+=static_cast<std::uint32_t>(length);
        if(length){
            n.phase=Phase::copy;n.pc=0x30c9fc;n.pc=0x30ca00;
            n.pc=0x30b3f0;s.flush.entered=true; //Fresh32-byte frame for EACH nonempty%s.
            n.pc=0x30b3f4;s.flush.saves|=2u;n.pc=0x30b3f8;s.flush.saves|=1u;
            n.pc=0x30b3fc;n.pc=0x30b400;n.pc=0x30b404;n.pc=0x30b408;n.pc=0x30b414;n.pc=0x30b418;
            n.pc=0x304df0;s.writer.entered=true; //Fresh144-byte writer; flag0x200 bypasses callbacks.
            n.pc=0x304df4;s.writer.saves|=4u;n.pc=0x304df8;s.writer.saves|=128u;
            n.pc=0x304dfc;n.pc=0x304e00;s.writer.saves|=1u;n.pc=0x304e04;s.writer.saves|=2u;
            n.pc=0x304e08;s.writer.saves|=8u;n.pc=0x304e0c;s.writer.saves|=16u;n.pc=0x304e10;s.writer.saves|=32u;
            n.pc=0x304e14;s.writer.saves|=256u;n.pc=0x304e18;s.writer.saves|=64u;
            n.pc=0x304f28;n.pc=0x304f40;
            if(length>*n.remaining08||length>=n.output940.size()-n.copied)
                return finish(WaterPoolPathFormatResult::Status::nativeAdmissionStopped);
            n.pc=0x306370;for(std::size_t i=0;i<length;++i)n.output940[n.copied+i]=s.bytes[i];
            n.copied+=length;n.known=n.copied;s.copied=true;
            n.pc=0x304f58;*n.remaining08-=static_cast<std::uint32_t>(length);
            n.pc=0x304f5c;*n.cursor00=static_cast<std::uint32_t>(n.copied);
            n.pc=0x305024;n.uioResidual=0u;n.pc=0x305028;n.pc=0x30502c;s.writerReturn=0u;
            for(std::uint32_t i=0;i<9u;++i){n.pc=0x30519cu+i*4u;s.writer.restores|=std::uint16_t(1u<<i);}
            n.pc=0x3051c0;n.pc=0x3051c4;s.writer.released=true;
            n.pc=0x30b41c;n.uioCount=0u;n.pc=0x30b420;n.uioResidual=0u;
            n.pc=0x30b424;s.flush.restores|=1u;n.pc=0x30b428;s.flush.restores|=2u;
            n.pc=0x30b42c;n.pc=0x30b430;s.flush.released=true;s.flushReturn=0u;
            n.pc=0x30ca04;n.pc=0x30ca08;
        }
        //Empty residual skips flush but STILL clears the enqueued zero vector.
        n.pc=0x30ca0c;n.uioCount=0u;n.pc=0x30ca10;n.pc=0x30ca14;s.completed=true;++n.completed;
    }
    if(!scan(2u))return finish(WaterPoolPathFormatResult::Status::nativeAdmissionStopped);
    n.pc=0x30ca18;n.pc=0x30ca1c;n.pc=0x30ca20;n.pc=0x30ca34; //Empty queue: no final flush.
    n.pc=0x30ca38;n.pc=0x30ca3c;n.pc=0x30ca40;n.pc=0x30ca44;n.pc=0x30ca48;n.pc=0x30ca4c;
    n.formatterReturn=*n.formatterCount; //Exact flags0x208 keep source error bit0x40 clear.
    for(std::uint32_t i=0;i<10u;++i){n.pc=0x30ca50u+i*4u;n.core.restores|=std::uint16_t(1u<<i);}
    n.pc=0x30ca78;n.pc=0x30ca7c;n.core.released=true;
    for(std::uint32_t i=0;i<4u;++i){n.pc=0x30b550u+i*4u;n.stdio.restores|=std::uint16_t(1u<<i);}
    n.pc=0x30b560;n.pc=0x30b564;n.stdio.released=true;
    n.pc=0x308cec;n.pc=0x308cf0;n.raRestored=true;
    n.pc=0x308cf4;n.output940[*n.cursor00]=0;n.known=n.copied+1u;n.terminated=true; //No FILE cursor/count increment.
    n.pc=0x308cf8;n.pc=0x308cfc;n.wrapperReleased=true;n.wrapperReturn=n.formatterReturn;
    n.pc=0x2b9d0c;n.phase=Phase::callerBoundary; //Before new operator-new occurrence and its1D0 delay argument.
    return finish(WaterPoolPathFormatResult::Status::nativeCallerBoundary);
}

#include "original_source_world_water_pool_allocation50la.inl"

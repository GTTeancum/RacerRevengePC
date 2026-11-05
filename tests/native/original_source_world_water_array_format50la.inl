bool Owner::preflightNativeWaterArrayFormat()const noexcept {
    if(!preflightNativeWaterArraySelection())return false;
    const auto& n=sourceWaterConfig_.nativeWaterArrayFormat;const auto& c=sourceWaterConfig_.nativeWaterArrayCaller;
    using Phase=SourceWaterConfig::NativeWaterArrayFormat::Phase;using G=a::OriginalConfigProcessGlobals50LA;
    const auto noFile=[&]()noexcept {return !n.fileReent54&&!n.flags0C&&!n.buffer10&&!n.bufferSize14&&!n.remaining08&&
        !n.cursor00&&!n.savedArgument&&!n.varargsSaveMask&&!n.raSaved;};
    const auto noFlush=[&]()noexcept {return !n.flushCallIssued&&!n.flushFrame&&!n.writerFrame&&
        !n.flushStackReleased&&!n.writerStackReleased&&!n.flushSaveMask&&!n.flushRestoreMask&&
        !n.writerSaveMask&&!n.writerRestoreMask&&!n.uioCount&&!n.uioResidual&&!n.writerReturn&&!n.flushReturn;};
    const auto noConversion=[&]()noexcept {return !n.conversionRead&&!n.decimalArgument&&!n.digitStored&&!n.digit&&noFlush();};
    const auto noReturn=[&]()noexcept {return !n.formatterReturn&&!n.wrapperReturn&&!n.terminated&&!n.raRestored&&
        !n.wrapperStackReleased&&!n.stdioStackReleased&&!n.coreStackReleased&&!n.stdioRestoreMask&&!n.coreRestoreMask&&!n.configArgument&&!n.nameArgument&&!n.selectionCallIssued&&!n.selectionReturnPC&&!n.configIdentity;};
    if(n.scanCount>n.scans.size())return false;
    for(std::size_t i=n.scanCount;i<n.scans.size();++i)
        if(n.scans[i].offset||n.scans[i].returned||n.scans[i].length||n.scans[i].wide)return false;
    if(!n.attempted)return n.phase==Phase::unentered&&n.pc==0u&&!n.role&&!n.wrapperFrame&&!n.capturedReent&&noFile()&&
        !n.stdioFrame&&!n.coreFrame&&!n.stdioSaveMask&&!n.coreSaveMask&&
        !n.initializerReturned&&!n.localeReturned&&!n.scanCount&&!n.segmentCount&&!n.copied&&!n.known&&
        !n.copiedSegments[0]&&!n.copiedSegments[1]&&noConversion()&&noReturn();
    if(!c.attempted||!c.formatCallIssued||c.pc!=0x308c90u||c.formatIndex!=1u||c.formatStackOffset!=0x710u||
       !n.role||n.role!=c.role||*n.role==NativeWaterArrayRole::zoneRaw||!n.wrapperFrame||
       !sourceWaterConfigOwner_||sourceWaterConfigOwner_->pc_!=0x30492cu||sourceWaterConfigOwner_->wakeSelection_.returned||
       (!sourceWaterProvider274720_.nativeWithdrawn&&(!sourceWaterProvider_||!worldApplication_)))return false;
    const bool signature=*n.role==NativeWaterArrayRole::signaturePointers;
    constexpr char signatureFormat[]="Signature Wave %d",poolFormat[]="Pool %d";
    const auto* format=signature?signatureFormat:poolFormat;const std::size_t prefix=signature?15u:5u;
    if(c.formatLiteral!=(signature?0x3c0af0u:0x3c0c20u)||c.formatReturnPC!=(signature?0x2b98fcu:0x2b9c78u)||
       n.segmentCount>2u||n.copied!=(n.segmentCount?prefix+n.segmentCount-1u:0u)||
       n.copiedSegments[0]!=(n.segmentCount?prefix:0u)||n.copiedSegments[1]!=(n.segmentCount==2u?1u:0u)||
       n.known!=n.copied+(n.terminated?1u:0u)||n.known>n.output.size())return false;
    for(std::size_t i=0;i<n.known;++i){
        const char expected=i<prefix?format[i]:i==prefix?'1':0;
        if(n.output[i]!=expected)return false;
    }
    if(!n.capturedReent)return n.phase==Phase::reent&&n.pc==0x308c98u&&noFile()&&!n.stdioFrame&&!n.coreFrame&&!n.stdioSaveMask&&!n.coreSaveMask&&
        !n.initializerReturned&&!n.localeReturned&&!n.scanCount&&!n.segmentCount&&noConversion()&&noReturn();
    if(n.capturedReent!=G::StdioIdentity::reent385160||n.fileReent54!=n.capturedReent||n.flags0C!=0x208u||
       !n.buffer10||n.bufferSize14!=0x7fffffffu||n.remaining08!=0x7fffffffu-n.copied||
       n.cursor00!=n.copied||n.savedArgument!=1u||n.varargsSaveMask!=63u||!n.raSaved||
       !n.stdioFrame||n.stdioSaveMask!=15u)return false;
    if(n.phase==Phase::stdio)return !n.initializerReturned&&!n.coreFrame&&!n.coreSaveMask&&!n.localeReturned&&!n.scanCount&&!n.segmentCount&&
        noConversion()&&noReturn()&&(n.pc==0x30b50cu||n.pc==0x30b528u||n.pc==0x30b534u);
    if(!n.initializerReturned||!n.coreFrame||n.coreSaveMask!=1023u)return false;
    if(n.phase==Phase::locale)return !n.localeReturned&&!n.scanCount&&!n.segmentCount&&noConversion()&&noReturn()&&
        (n.pc==0x305330u||n.pc==0x3052f0u||n.pc==0x30b5a8u);
    if(!n.localeReturned||!n.scanCount||n.scanCount>prefix+2u)return false;
    bool scanFailure=false;
    for(std::size_t i=0;i<n.scanCount;++i){
        const auto& s=n.scans[i];const auto offset=i<=prefix?i:prefix+2u;
        const auto byte=static_cast<unsigned char>(format[offset]);
        if(s.offset!=offset)return false;
        if(!s.returned){
            if(i+1u!=n.scanCount||s.length||s.wide||(n.pc!=0x30b658u&&n.pc!=0x30b65cu))return false;
            scanFailure=true;
        }else if(s.length==-1){
            if(i+1u!=n.scanCount||s.wide||n.pc!=0x30b674u)return false;scanFailure=true;
        }else if(s.length!=(byte?1:0)||s.wide!=std::uint32_t(byte))return false;
    }
    if(n.phase==Phase::prefixScan)return scanFailure&&n.scanCount<=prefix+1u&&!n.segmentCount&&noConversion()&&noReturn();
    if(n.scanCount<prefix+1u||!n.conversionRead||n.decimalArgument!=1u||!n.digitStored||!n.flushCallIssued||n.digit!=static_cast<unsigned char>('1'))return false;
    if(!n.flushFrame||!n.writerFrame||n.flushSaveMask!=3u||n.writerSaveMask!=511u)return false;
    if(n.phase==Phase::prefixCopy||n.phase==Phase::digitCopy)return !scanFailure&&n.scanCount==prefix+1u&&
        n.segmentCount==(n.phase==Phase::prefixCopy?0u:1u)&&n.pc==0x304f40u&&noReturn()&&
        n.uioCount==2u&&n.uioResidual==prefix+1u-n.copied&&!n.writerReturn&&!n.flushReturn&&
        !n.writerRestoreMask&&!n.flushRestoreMask&&!n.writerStackReleased&&!n.flushStackReleased;
    if(n.segmentCount!=2u||n.scanCount!=prefix+2u||n.uioCount!=0u||n.uioResidual!=0u||
       n.writerReturn!=0u||n.flushReturn!=0u||n.writerRestoreMask!=511u||n.flushRestoreMask!=3u||
       !n.writerStackReleased||!n.flushStackReleased)return false;
    if(n.phase==Phase::nulScan)return scanFailure&&noReturn();
    return n.phase==Phase::selectionEntry&&!scanFailure&&n.formatterReturn==prefix+1u&&
        n.wrapperReturn==n.formatterReturn&&n.terminated&&n.raRestored&&n.wrapperStackReleased&&
        n.coreRestoreMask==1023u&&n.stdioRestoreMask==15u&&n.coreStackReleased&&n.stdioStackReleased&&
        n.configArgument&&n.nameArgument&&n.selectionCallIssued&&n.configIdentity==sourceWaterConfigOwner_.get()&&
        n.selectionReturnPC==(signature?0x2b9908u:0x2b9c84u)&&n.pc==0x157d00u;
}
Owner::WaterArrayFormatResult Owner::formatWaterArraySectionNativeOnly(WaterArrayBackingMode mode)noexcept {
    WaterArrayFormatResult result;
    const auto& h=sourceWaterConfig_;const auto& r=sourceWaterProviderInitializer_;
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||closed_||
       nativeScbThread_!=std::this_thread::get_id()||runtimeCameraBorrows_||runtimeWorldBorrows_||
       constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||
       !h.wakeSelectionEntered||h.nativeWithdrawn||sourceWaterProvider274720_.nativeWithdrawn||
       !sourceWaterConfigOwner_||!sourceWaterProvider_||!worldApplication_||!sourceLoadApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||
       r.pc!=0x30492cu||sourcePostParticleModelsPC_!=r.pc||sourceWaterProvider274720_.pc!=r.pc||
       sourceWaterProviderNumeric_.pc!=r.pc||!preflightSourceWaterConfigNative())return result;
    auto& n=sourceWaterConfig_.nativeWaterArrayFormat;const auto& c=sourceWaterConfig_.nativeWaterArrayCaller;
    result.originalSourceStop=sourceWaterConfigOwner_->pc_;result.allocatorStop=sourceWaterConfig_.nativeWaterAllocationEntry.pc;
    if(!c.attempted||!c.formatCallIssued||c.pc!=0x308c90u||!c.role||*c.role==NativeWaterArrayRole::zoneRaw||
       c.formatIndex!=1u||c.formatStackOffset!=0x710u)return result;
    const auto finish=[&](WaterArrayFormatResult::Status status)noexcept {
        result.status=status;result.adaptedPC=n.pc;result.formatterReturn=n.formatterReturn;
        result.knownOutputBytes=n.known;result.selectionCallIssued=n.selectionCallIssued;return result;
    };
    if(n.attempted)return finish(WaterArrayFormatResult::Status::alreadyAttempted);
    using Phase=SourceWaterConfig::NativeWaterArrayFormat::Phase;using G=a::OriginalConfigProcessGlobals50LA;
    const G::WaterSprintfAccess access;
    const bool signature=*c.role==NativeWaterArrayRole::signaturePointers;
    constexpr char signatureFormat[]="Signature Wave %d",poolFormat[]="Pool %d";
    const auto* format=signature?signatureFormat:poolFormat;const std::size_t prefix=signature?15u:5u;
    n.attempted=true;n.role=c.role;n.phase=Phase::reent;n.pc=0x308c90;
    n.pc=0x308c94;n.wrapperFrame=true; //224-byte symbolic frame, no guessed unused register bits.
    G::StdioIdentity captured;
    if(!G::readWaterReent308C98(access,captured,n.pc))return finish(WaterArrayFormatResult::Status::nativeAdmissionStopped);
    n.capturedReent=captured;n.pc=0x308c9c;n.pc=0x308ca0;n.pc=0x308ca4;
    n.pc=0x308ca8;n.savedArgument=*c.formatIndex;n.varargsSaveMask|=1u;
    n.pc=0x308cac;n.pc=0x308cb0;n.raSaved=true;
    n.pc=0x308cb4;n.pc=0x308cb8;n.varargsSaveMask|=2u; //Unused a3 remains unknown.
    n.pc=0x308cbc;n.pc=0x308cc0;n.varargsSaveMask|=4u;
    n.pc=0x308cc4;n.varargsSaveMask|=8u;n.pc=0x308cc8;n.varargsSaveMask|=16u;
    n.pc=0x308ccc;n.varargsSaveMask|=32u; //Unused t0..t3 remain unknown.
    n.pc=0x308cd0;n.flags0C=0x208u;
    n.pc=0x308cd4;n.buffer10=true; //Same symbolic caller stack710 output role.
    n.pc=0x308cd8;n.bufferSize14=0x7fffffffu;
    n.pc=0x308cdc;n.fileReent54=captured;
    n.pc=0x308ce0;n.cursor00=0u;
    n.pc=0x308ce4;n.pc=0x308ce8;n.remaining08=0x7fffffffu;
    n.phase=Phase::stdio;n.pc=0x30b4f0;n.stdioFrame=true; //64-byte frame.
    n.pc=0x30b4f4;n.stdioSaveMask|=2u;n.pc=0x30b4f8;n.stdioSaveMask|=4u;
    n.pc=0x30b4fc;n.pc=0x30b500;n.stdioSaveMask|=8u;n.pc=0x30b504;n.stdioSaveMask|=1u;n.pc=0x30b508;
    if(!G::initializeWaterSprintf30B528(access,*n.fileReent54,n.pc))return finish(WaterArrayFormatResult::Status::nativeAdmissionStopped);
    n.initializerReturned=true;n.phase=Phase::locale;n.pc=0x30b568;n.coreFrame=true; //720-byte frame.
    n.pc=0x30b56c;n.coreSaveMask|=32u;n.pc=0x30b570;n.coreSaveMask|=512u;
    n.pc=0x30b574;n.pc=0x30b578;n.pc=0x30b57c;n.pc=0x30b580;n.coreSaveMask|=1u;
    n.pc=0x30b584;n.coreSaveMask|=2u;n.pc=0x30b588;n.coreSaveMask|=4u;n.pc=0x30b58c;n.coreSaveMask|=8u;
    n.pc=0x30b590;n.coreSaveMask|=16u;n.pc=0x30b594;n.coreSaveMask|=64u;n.pc=0x30b598;n.coreSaveMask|=128u;
    n.pc=0x30b59c;n.coreSaveMask|=256u;n.pc=0x30b5a0;n.pc=0x30b5a4;
    char unusedDecimal;
    if(!G::waterLocale305328(access,unusedDecimal,n.pc))return finish(WaterArrayFormatResult::Status::nativeAdmissionStopped);
    n.localeReturned=true;
    const auto scan=[&](std::size_t offset)noexcept {
        auto& s=n.scans[n.scanCount++];s.offset=offset;
        const auto byte=static_cast<unsigned char>(format[offset]);std::uint32_t wide=0;std::int32_t length=0;
        if(!G::waterByte306108(access,byte,wide,length,n.pc))return false;
        s.returned=true;s.length=length;if(length>=0)s.wide=wide;
        if(length!=(byte?1:0)){n.pc=0x30b674;return false;}return true;
    };
    n.phase=Phase::prefixScan;
    for(std::size_t i=0;i<=prefix;++i)if(!scan(i))return finish(WaterArrayFormatResult::Status::nativeAdmissionStopped);
    n.pc=0x30b718;n.conversionRead=true; //Immutable 'd' is dispatch-read directly, not306108-scanned.
    n.pc=0x30b948;n.decimalArgument=static_cast<std::uint32_t>(*n.savedArgument);
    n.pc=0x30b94c; //The retained exact argument1 takes the nonnegative signed path.
    n.pc=0x30bce4;n.pc=0x30bce8;n.pc=0x30bd64;n.pc=0x30bd68;
    n.pc=0x30bdb8;n.pc=0x30bdbc;n.pc=0x30bdc0;
    const auto digit=std::uint64_t(*n.decimalArgument)+0x30u;
    n.pc=0x30bdc4;n.pc=0x30bdc8;n.pc=0x30bdcc;n.pc=0x30bdd0;
    n.digit=static_cast<unsigned char>(digit&0xffu);n.digitStored=true; //Single-digit fast path, NO division helpers.
    const auto copy=[&](const char* bytes,std::size_t size)noexcept {
        n.pc=0x304f28; //Two segments of the SAME flush; known string flag bypasses callbacks.
        n.pc=0x304f40;
        if(n.segmentCount>=n.copiedSegments.size()||!n.remaining08||!n.cursor00||n.flags0C!=0x208u||
           size>*n.remaining08||n.copied>=n.output.size()||size>=n.output.size()-n.copied)return false;
        n.pc=0x306370;for(std::size_t i=0;i<size;++i)n.output[n.copied+i]=bytes[i];
        n.copied+=size;n.known=n.copied;
        n.pc=0x304f58;*n.remaining08-=static_cast<std::uint32_t>(size);
        n.pc=0x304f5c;*n.cursor00+=size;
        n.copiedSegments[n.segmentCount++]=size;
        n.pc=0x304f60;n.pc=0x304f64;n.pc=0x305010;n.pc=0x305014;n.pc=0x305018;n.pc=0x30501c;
        n.pc=0x305020;n.pc=0x305024;n.uioResidual=static_cast<std::uint32_t>(prefix+1u-n.copied);return true;
    };
    n.phase=Phase::prefixCopy;n.uioCount=2u;n.uioResidual=static_cast<std::uint32_t>(prefix+1u);
    n.pc=0x30c9fc;n.flushCallIssued=true;n.pc=0x30ca00; //One flush contains prefix then digit.
    n.pc=0x30b3f0;n.flushFrame=true;n.pc=0x30b3f4;n.flushSaveMask|=2u;n.pc=0x30b3f8;n.flushSaveMask|=1u;
    n.pc=0x30b3fc;n.pc=0x30b400;n.pc=0x30b404;n.pc=0x30b408;n.pc=0x30b414;n.pc=0x30b418;
    n.pc=0x304df0;n.writerFrame=true; //144-byte frame.
    n.pc=0x304df4;n.writerSaveMask|=4u;n.pc=0x304df8;n.writerSaveMask|=128u;
    n.pc=0x304dfc;n.pc=0x304e00;n.writerSaveMask|=1u;n.pc=0x304e04;n.writerSaveMask|=2u;
    n.pc=0x304e08;n.writerSaveMask|=8u;n.pc=0x304e0c;n.writerSaveMask|=16u;n.pc=0x304e10;n.writerSaveMask|=32u;
    n.pc=0x304e14;n.writerSaveMask|=256u;n.pc=0x304e18;n.writerSaveMask|=64u;
    if(!copy(format,prefix))return finish(WaterArrayFormatResult::Status::nativeAdmissionStopped);
    n.phase=Phase::digitCopy;const char digitByte=static_cast<char>(*n.digit);
    if(!copy(&digitByte,1u))return finish(WaterArrayFormatResult::Status::nativeAdmissionStopped);
    n.pc=0x305028;n.pc=0x30502c;n.writerReturn=0u;
    for(std::uint32_t i=0;i<9u;++i){n.pc=0x30519cu+i*4u;n.writerRestoreMask|=std::uint16_t(1u<<i);}
    n.pc=0x3051c0;n.pc=0x3051c4;n.writerStackReleased=true;
    n.pc=0x30b41c;n.uioCount=0u;n.pc=0x30b420;n.uioResidual=0u;
    n.pc=0x30b424;n.flushRestoreMask|=1u;n.pc=0x30b428;n.flushRestoreMask|=2u;
    n.pc=0x30b42c;n.pc=0x30b430;n.flushStackReleased=true;n.flushReturn=0u;
    n.pc=0x30ca04;n.pc=0x30ca08;n.pc=0x30ca0c;n.pc=0x30ca10;n.pc=0x30ca14;
    n.phase=Phase::nulScan;
    if(!scan(prefix+2u))return finish(WaterArrayFormatResult::Status::nativeAdmissionStopped);
    n.pc=0x30ca18;n.pc=0x30ca1c;n.pc=0x30ca20;n.pc=0x30ca34;n.pc=0x30ca38;n.pc=0x30ca3c; //Empty uio skips a second flush.
    n.pc=0x30ca40;n.pc=0x30ca44;n.pc=0x30ca48;n.pc=0x30ca4c;
    n.formatterReturn=static_cast<std::uint32_t>(n.copied); //Known flags0x208 have error0x40 clear.
    for(std::uint32_t i=0;i<10u;++i){n.pc=0x30ca50u+i*4u;n.coreRestoreMask|=std::uint16_t(1u<<i);}
    n.pc=0x30ca78;n.pc=0x30ca7c;n.coreStackReleased=true;
    for(std::uint32_t i=0;i<4u;++i){n.pc=0x30b550u+i*4u;n.stdioRestoreMask|=std::uint16_t(1u<<i);}
    n.pc=0x30b560;n.pc=0x30b564;n.stdioStackReleased=true;
    n.pc=0x308cec;n.pc=0x308cf0;n.raRestored=true;
    n.pc=0x308cf4;n.output[*n.cursor00]=0;n.known=n.copied+1u;n.terminated=true; //No FILE cursor/count increment.
    n.pc=0x308cf8;n.pc=0x308cfc;n.wrapperStackReleased=true;n.wrapperReturn=n.formatterReturn;
    n.pc=*c.formatReturnPC;n.configArgument=true;n.configIdentity=sourceWaterConfigOwner_.get(); //Caller stack240, SAME Config.
    n.pc=signature?0x2b9900u:0x2b9c7cu;n.selectionCallIssued=true;
    n.selectionReturnPC=signature?0x2b9908u:0x2b9c84u;
    n.pc=signature?0x2b9904u:0x2b9c80u;n.nameArgument=true; //Same known caller stack710 name role.
    n.pc=0x157d00;n.phase=Phase::selectionEntry;
    return finish(WaterArrayFormatResult::Status::nativeSelectionEntry); //Selection body/teardown untouched.
}

#include "original_source_world_water_array_selection50la.inl"

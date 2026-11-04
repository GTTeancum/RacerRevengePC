//Included inside the actual World namespace. Source strings remain one App
//process owner; this frame's copies are historical, never live path aliases.
bool Owner::preflightSourceWaterProviderMembersNative()const noexcept {
    const auto& m=sourceWaterProviderMembers_;const auto& h=sourceWaterProvider274720_;
    if(!preflightSourceWaterProviderNumericNative())return false;
    using G=a::OriginalConfigProcessGlobals50LA;
    if(m.scansEntered>3||m.scansReturned>m.scansEntered||m.scansEntered>m.scansReturned+1||
       m.segmentCount>2||m.embeddedWritten>4)return false;
    if(!m.entered){
        if(m.capturedReent||m.fileReent54||m.fileFlags0C||m.fileCapacity14||m.fileRemaining08||
           m.fileBuffer10||m.fileCursor00||m.priorNativeStdioBounded||m.stdioInitialized||m.localeReturned||m.scansEntered||
           m.trackLength||m.suffixLength||m.pathLength||m.pathPublication||m.formatReturnV0||
           m.copied||m.segmentCount||m.terminated||m.formatReturned||m.baseReturned||m.embeddedReturned||
           m.vectorArrayReturned||m.zeroArrayReturned||m.integerPrefixComplete||m.embeddedWritten||
           m.rawF0||m.rawF1||m.field380||m.field384||m.fieldD8||m.field388||m.fieldE0||m.fieldDC)return false;
    }else if(!h.formatCallEntered||!h.waterBaseEntered||!h.countAcquired)return false;
    if(m.capturedReent){
        if(!m.priorNativeStdioBounded||*m.capturedReent!=G::StdioIdentity::reent385160||m.fileReent54!=m.capturedReent||
           m.fileFlags0C!=0x208u||m.fileCapacity14!=0x7fffffffu||!m.fileBuffer10||
           !m.fileCursor00||*m.fileCursor00!=m.copied||m.fileRemaining08!=0x7fffffffu-m.copied)return false;
    }else if(m.fileReent54||m.fileFlags0C||m.fileCapacity14||m.fileRemaining08||m.fileBuffer10||
       m.fileCursor00||m.stdioInitialized||m.localeReturned||m.scansEntered||m.copied)return false;
    if((m.stdioInitialized&&!m.capturedReent)||(m.localeReturned&&!m.stdioInitialized)||
       (m.scansEntered&&!m.localeReturned))return false;
    constexpr unsigned char bytes[3]={'%','%',0};
    for(std::size_t n=0;n<3;++n){const auto& scan=m.scans[n];
        if(n<m.scansReturned){if(scan.length!=(bytes[n]?1:0)||scan.wide!=bytes[n])return false;}
        else if(n>=m.scansEntered){if(scan.length||scan.wide)return false;}
        else if(scan.length&&*scan.length!=-1)return false;
    }
    const auto validText=[](const auto& text,const auto& length)noexcept {
        if(!length)return true;if(*length>=text.size()||text[*length])return false;
        for(std::size_t n=0;n<*length;++n)if(!text[n])return false;return true;
    };
    if(!validText(m.trackPrefix,m.trackLength)||!validText(m.waterSuffix,m.suffixLength)||
       m.trackLength!=m.stringLengths[0]||m.suffixLength!=m.stringLengths[1]||
       (m.trackLength&&m.scansReturned<1)||(m.suffixLength&&(m.scansReturned<2||!m.trackLength)))return false;
    std::size_t copied=0,segments=0;
    for(std::size_t n=0;n<2;++n)if(m.copiedSegments[n]){
        if(!m.stringLengths[n]||!*m.copiedSegments[n]||m.copiedSegments[n]!=m.stringLengths[n]||
           (n&&m.stringLengths[0]&&*m.stringLengths[0]&&!m.copiedSegments[0]))return false;
        copied+=*m.copiedSegments[n];++segments;
    }
    if(copied!=m.copied||segments!=m.segmentCount||m.copied>=m.formattedPath.size()||
       ((m.copied||m.terminated)&&!m.pathPublication))return false;
    if(m.pathPublication){
        if(!*m.pathPublication||(!m.copied&&!m.terminated)||!m.entered)return false;
        if(m.copiedSegments[0]&&std::memcmp(m.formattedPath.data(),m.trackPrefix.data(),*m.copiedSegments[0]))return false;
        if(m.copiedSegments[1]&&std::memcmp(m.formattedPath.data()+*m.trackLength,m.waterSuffix.data(),*m.copiedSegments[1]))return false;
    }
    if(m.formatReturned!=m.baseReturned||m.formatReturned!=m.integerPrefixComplete||
       m.formatReturned!=m.terminated||bool(m.formatReturnV0)!=m.formatReturned||bool(m.pathLength)!=m.formatReturned)return false;
    if(m.formatReturned){
        if(m.scansReturned!=3||!m.trackLength||!m.suffixLength||*m.pathLength!=*m.trackLength+*m.suffixLength||
           *m.pathLength!=m.copied||m.formatReturnV0!=m.copied||m.formattedPath[m.copied]!=0||
           !m.embeddedReturned||!m.vectorArrayReturned||!m.zeroArrayReturned||m.embeddedWritten!=4||
           m.rawF0!=0x40000000u||m.rawF1!=0xc0000000u||
           h.pc!=(sourceWaterProviderNumeric_.entered?sourceWaterProviderNumeric_.pc:0x2b84c0u))return false;
        const auto& args=sourceWaterFactory274570_.arguments;const auto& layout=sourceWaterFactory274570_.numericLayout;
        if(!args||!layout||m.field380!=args->constructorA2||m.fieldD8!=args->constructorA3||
           m.field384!=layout->sourceColumns384AndDCWord()||m.fieldDC!=m.field384||
           m.field388!=layout->sourceWords388Word()||m.fieldE0!=layout->sourceFieldE0Word())return false;
    }else if(m.baseReturned||m.embeddedReturned||m.vectorArrayReturned||m.zeroArrayReturned||m.embeddedWritten||
       m.rawF0||m.rawF1||m.field380||m.field384||m.fieldD8||m.field388||m.fieldE0||m.fieldDC)return false;
    for(std::size_t n=0;n<4;++n)
        if(m.embeddedWords[n]!=(m.formatReturned?std::optional<std::uint32_t>(n?0u:0xffffffffu):std::nullopt))return false;
    constexpr std::array<std::uint32_t,4> vf0{{0,0,0,0x3f800000u}};
    for(const auto& vector:m.vectors)if(vector!=(m.formatReturned?std::optional<std::array<std::uint32_t,4>>(vf0):std::nullopt))return false;
    for(std::size_t n=0;n<2;++n){
        if(m.integerArrayWritten[n]!=(m.formatReturned?15u:0u))return false;
        for(const auto& lane:m.integerArrays[n])if(lane!=(m.formatReturned?std::optional<std::uint32_t>(0):std::nullopt))return false;
    }
    if(h.nativeWithdrawn||!m.pathPublication)return true;
    if(!worldApplication_)return false;const auto& p=worldApplication_->sourceWaterProcessPaths_;
    if(p.publication<*m.pathPublication)return false;
    //Later legitimate writes, or explicitly invalidated unknown writers, do
    //not turn the earlier snapshot into a current-path requirement.
    if(!p.unknown&&p.publication==*m.pathPublication&&p.directory36BFD0!=m.formattedPath)return false;
    return true;
}
bool Owner::continueSourceWaterProviderFormat308C90(std::uint32_t& pc)noexcept {
    auto& h=sourceWaterProvider274720_;auto& m=sourceWaterProviderMembers_;
    if(pc!=0x308c90u||h.pc!=pc||sourcePostParticleModelsPC_!=pc||m.entered||
       !h.formatCallEntered||!h.countAcquired||!sourceWaterProvider_||!sourceLoadApplication_||!worldApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||nativeScbThread_!=std::this_thread::get_id()||
       !preflightSourcePostParticleModelsNative()||!preflightSourceWorldParticleCountNative())return false;
    const auto at=[&](std::uint32_t p)noexcept{pc=sourcePostParticleModelsPC_=h.pc=p;};
    const auto stop=[&](std::uint32_t p)noexcept{at(p);return false;};
    m.entered=true;at(0x308c90);
    const auto& prefix=sourceLoadApplication_->trackDirectory36B640_;
    if(!worldApplication_->waterDirectory2B095CAdmitted(prefix))return false;
    auto& paths=worldApplication_->sourceWaterProcessPaths_;
    using G=a::OriginalConfigProcessGlobals50LA;const G::WaterSprintfAccess access;
    G::worldNativeStdioAdapterBoundary(access);m.priorNativeStdioBounded=true;
    G::StdioIdentity captured;
    if(!G::readWaterReent308C98(access,captured,pc))return stop(pc);
    m.capturedReent=captured;
    at(0x308cd0);m.fileFlags0C=0x208u;at(0x308cd4);m.fileBuffer10=true;
    at(0x308cd8);m.fileCapacity14=0x7fffffffu;at(0x308cdc);m.fileReent54=captured;
    at(0x308ce0);m.fileCursor00=0;at(0x308ce4);at(0x308ce8);m.fileRemaining08=0x7fffffffu;
    if(!G::initializeWaterSprintf30B528(access,captured,pc))return stop(pc);m.stdioInitialized=true;
    char decimal;if(!G::waterLocale305328(access,decimal,pc))return stop(pc);m.localeReturned=true;
    const auto scan=[&](std::size_t index,unsigned char byte)noexcept {
        auto& receipt=m.scans[index];++m.scansEntered;std::uint32_t wide=0;std::int32_t length=0;
        if(!G::waterByte306108(access,byte,wide,length,pc))return stop(pc);
        receipt.length=length;if(length>=0)receipt.wide=wide;
        if(length!=(byte?1:0))return stop(0x30b674);
        ++m.scansReturned;at(pc);return true;
    };
    const auto beginWrite=[&]()noexcept {
        if(!m.pathPublication)m.pathPublication=++paths.publication;
    };
    const auto copy=[&](std::size_t index,const char* input,std::size_t length)noexcept {
        //Zero-size%s enqueues an iovec but residual0 skips30C9FC/304F28.
        if(!length)return true;
        if(!m.fileRemaining08||length>*m.fileRemaining08||length>=paths.directory36BFD0.size()-m.copied)return stop(0x304f40);
        at(0x30c9fc);at(0x304f28);at(0x304f40);at(0x306370);beginWrite();
        std::memcpy(paths.directory36BFD0.data()+m.copied,input,length);
        at(0x304f58);*m.fileRemaining08-=static_cast<std::uint32_t>(length);
        at(0x304f5c);m.copied+=length;*m.fileCursor00=m.copied;
        m.copiedSegments[index]=length;++m.segmentCount;m.formattedPath=paths.directory36BFD0;return true;
    };
    //Only offsets0('%'),2('%'),4(NUL) pass306108. The immutable 's'
    //specifier bytes at1/3 are direct parser reads, not extra306108 calls.
    if(!scan(0,'%'))return false;at(0x30b718);at(0x30bbd4);at(0x3093a8);
    std::memcpy(m.trackPrefix.data(),prefix.data(),prefix.size());m.trackPrefix[prefix.size()]=0;
    m.trackLength=prefix.size();m.stringLengths[0]=prefix.size();
    if(!copy(0,m.trackPrefix.data(),*m.trackLength))return false;
    if(!scan(1,'%'))return false;at(0x30b718);at(0x30bbd4);at(0x3093a8);
    std::size_t suffix=0;while(paths.suffix36BEC0[suffix])++suffix; //Whole-call bounded admission above.
    m.waterSuffix=paths.suffix36BEC0;m.suffixLength=suffix;m.stringLengths[1]=suffix;
    if(!copy(1,m.waterSuffix.data(),suffix))return false;
    if(!scan(2,0))return false;at(0x30ca40);m.formatReturnV0=static_cast<std::uint32_t>(m.copied);
    //Final308CF4 terminator neither advances the FILE cursor nor changes08.
    at(0x308cf4);beginWrite();paths.directory36BFD0[m.copied]=0;m.terminated=true;
    m.formattedPath=paths.directory36BFD0;m.pathLength=m.copied;
    at(0x308cf8);at(0x308cfc);m.formatReturned=true;
    at(0x2b0964);at(0x2b0968);at(0x2b096c);at(0x2b0970);m.baseReturned=true;
    at(0x2b8394);at(0x2b83a0);h.vtable08=0x3d46e0u;
    at(0x2b83a8);at(0x2b83ac);at(0x133f80);
    constexpr std::uint32_t embeddedPC[4]={0x133f84,0x133f88,0x133f8c,0x133f90};
    for(std::size_t n=0;n<4;++n){at(embeddedPC[n]);m.embeddedWords[n]=n?0u:0xffffffffu;++m.embeddedWritten;}
    at(0x133f94);at(0x133f98);m.embeddedReturned=true;
    constexpr std::array<std::uint32_t,4> vf0{{0,0,0,0x3f800000u}};
    at(0x2b83b8);m.vectors[0]=vf0;
    at(0x2b83d4);at(0x2b83d8);at(0x2e8860);
    for(std::size_t n=1;n<=2;++n){at(0x2e88cc);at(0x2e88d0);at(0x1048e0);m.vectors[n]=vf0;
        at(0x1048e4);at(0x1048e8);at(0x2e88d8);at(0x2e88e0);}
    at(0x2e8984);at(0x2e8988);m.vectorArrayReturned=true;
    constexpr std::uint32_t vectorPC[4]={0x2b83e4,0x2b83f0,0x2b83fc,0x2b8408};
    for(std::size_t n=0;n<4;++n){at(vectorPC[n]);m.vectors[n+3]=vf0;}
    at(0x2b8424);at(0x2b8428);at(0x2e8860);
    for(std::size_t n=0;n<2;++n){at(0x2e88cc);at(0x2e88d0);
        for(std::size_t lane=4;lane-->0;){at(0x2b8590u+4u*(3u-static_cast<std::uint32_t>(lane)));
            m.integerArrays[n][lane]=0;m.integerArrayWritten[n]|=std::uint8_t(1u<<lane);}
        at(0x2b85a0);at(0x2b85a4);at(0x2e88d8);at(0x2e88e0);}
    at(0x2e8984);at(0x2e8988);m.zeroArrayReturned=true;
    at(0x2b8434);m.vectors[7]=vf0;at(0x2b8440);m.vectors[8]=vf0;
    //Raw MTC1 bit transfers only. No float evaluation/conversion/division.
    at(0x2b844c);m.rawF1=0xc0000000u;at(0x2b8458);m.rawF0=0x40000000u;
    const auto& args=*sourceWaterFactory274570_.arguments;
    at(0x2b845c);m.field380=args.constructorA2;
    at(0x2b8474);m.field384=(*m.field380<<1u)+*m.field380+1u;
    at(0x2b847c);m.fieldD8=args.constructorA3;
    at(0x2b8498);m.field388=(((*m.field384<<1u)+*m.field384)<<3u)-*m.field384;
    at(0x2b84b0);m.fieldE0=((*m.fieldD8<<2u)+*m.fieldD8)<<2u;
    at(0x2b84bc);m.fieldDC=*m.field384;m.integerPrefixComplete=true;
    at(0x2b84c0);return continueSourceWaterProviderNumeric2B84C0(pc);
}
#include "original_source_world_water_provider_numeric50la.inl"

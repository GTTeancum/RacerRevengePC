OriginalWaterSourceConfig50LA::Text::Bytes OriginalWaterSourceConfig50LA::arrayFilenameDefaultNative()noexcept {
    Text::Bytes bytes;
    //Exact eight-byte immutable read extent at3C0B18, distinct from inner3A6288.
    for(std::size_t i=0;i<8u;++i)bytes.put(i,0u);return bytes;
}
bool OriginalWaterSourceConfig50LA::readArrayFilenameNativeOnly(ShaderBytes& destination,
    std::uint32_t callerReturnPC,D& d)noexcept {
    d={};auto& g=nativeArrayFilename_;const auto& s=nativeArraySelection_;
    if(g.entered||nativeWakeGetters_.active||nativeClosed_||!borrowed_||thread_!=std::this_thread::get_id()||
       pc_!=0x30492cu||(callerReturnPC!=0x2b9928u&&callerReturnPC!=0x2b9ca4u)||
       !preflightNative(waterReceiver_))return false;
    const bool array=nativeArrayLoad_.nativeLoaded;
    if(array){
        if(!nativeArrayLoad_.table||!nativeArrayLoad_.data||source_.child24!=&arrayTableRole_||
           nativeArrayLoad_.pointerZeroRole!=&arrayDataRole_)return false;
    }else if(s.returned!=false||nativeArrayRetirement_.attempted||nativeArrayLoad_.attempted||
        !nativeWakeLoad_.nativeLoaded||!nativeWakeLoad_.table||!nativeWakeLoad_.data||
        source_.child24!=&wakeTableRole_||nativeWakeLoad_.pointerZeroRole!=&wakeDataRole_)return false;
    g.entered=true;g.section=array?ArrayFilenameSection::array:ArrayFilenameSection::wake;
    g.count=source_.selected20;g.tableIdentity=source_.child24;
    g.dataIdentity=array?static_cast<const void*>(&arrayDataRole_):static_cast<const void*>(&wakeDataRole_);
    g.destinationIdentity=&destination;g.callerReturnPC=callerReturnPC;
    struct Scope {bool& active;explicit Scope(bool& value)noexcept:active(value){active=true;}
        ~Scope(){active=false;}} scope(g.active);
    auto& f=g.value;f.attempted=true;g.pc=0x158ad0;g.frame=true; //48-byte symbolic frame.
    g.pc=0x158ad4;g.saveMask|=4u;g.pc=0x158ad8;g.saveMask|=2u;g.pc=0x158adc;g.saveMask|=1u;
    g.pc=0x158ae0;g.pc=0x158ae4;g.pc=0x158ae8; //Original t0=-1 passes througha3; no source128 limit.
    g.pc=0x158aec;g.pc=0x158af0;
    //Native non-overlap aperture128 for caller+790, not truncation/default permission.
    if(!findWaterField158240(f,"filename",&destination,&f.lookupCopy,128u))return false;
    g.pc=0x158af4;g.pc=0x158af8;
    if(!*f.found){
        g.pc=0x158b08;const auto fallback=arrayFilenameDefaultNative();
        if(!copyShader309230(f.defaultCopy,fallback,0u,0u,8u,destination))return false;
        g.pc=0x158b10; //Second distinct NUL store after inner lookup's3A6288 empty copy.
    }else{g.pc=0x158afc;g.pc=0x158b00;}
    f.returned=*f.found;
    g.pc=0x158b14;g.restoreMask|=4u;g.pc=0x158b18;g.restoreMask|=2u;g.pc=0x158b1c;g.restoreMask|=1u;
    g.pc=0x158b20;g.pc=0x158b24;g.stackReleased=true;return true;
}
bool OriginalWaterSourceConfig50LA::preflightArrayFilenameNative()const noexcept {
    const auto& g=nativeArrayFilename_;const auto& f=g.value;
    if(g.active||!preflightShaderValueNative(f,g.count,arrayFilenameDefaultNative(),0u,128u))return false;
    if(!g.entered)return !g.frame&&!g.stackReleased&&!g.saveMask&&!g.restoreMask&&!g.pc&&!g.count&&
        !g.section&&!g.tableIdentity&&!g.dataIdentity&&!g.destinationIdentity&&!g.callerReturnPC&&!f.attempted;
    if(!g.frame||g.saveMask!=7u||!g.section||!g.destinationIdentity||!f.attempted||pc_!=0x30492cu||
       (g.callerReturnPC!=0x2b9928u&&g.callerReturnPC!=0x2b9ca4u)||nativeWakeGetters_.completed!=5u)return false;
    if(*g.section==ArrayFilenameSection::array){
        if(!nativeArrayLoad_.nativeLoaded||g.count!=nativeArrayLoad_.counted||
           g.tableIdentity!=&arrayTableRole_||g.dataIdentity!=&arrayDataRole_||
           !nativeArrayLoad_.table||!nativeArrayLoad_.data)return false;
    }else if(*g.section!=ArrayFilenameSection::wake||nativeArraySelection_.returned!=false||nativeArrayRetirement_.attempted||nativeArrayLoad_.attempted||
        !nativeWakeLoad_.nativeLoaded||g.count!=nativeWakeLoad_.counted||g.tableIdentity!=&wakeTableRole_||
        g.dataIdentity!=&wakeDataRole_||!nativeWakeLoad_.table||!nativeWakeLoad_.data)return false;
    if(source_.selected20!=g.count||source_.child24!=g.tableIdentity)return false;
    if(!f.returned)return !g.stackReleased&&!g.restoreMask;
    return g.stackReleased&&g.restoreMask==7u&&g.pc==0x158b24u;
}

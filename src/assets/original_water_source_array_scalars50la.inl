//First signature14 or pool3 caller-ordered scalar getters, own native PC only.
bool OriginalWaterSourceConfig50LA::readArrayScalarNative(std::size_t ordinal,bool signature,
    std::optional<std::uint32_t>& destination,D& d)noexcept {
    d={};auto& g=nativeArrayScalars_;const auto& filename=nativeArrayFilename_;
    if(ordinal>=(signature?14u:3u)||g.active||nativeArrayFilename_.active||nativeWakeGetters_.active||
       nativeClosed_||!borrowed_||thread_!=std::this_thread::get_id()||pc_!=0x30492cu||
       !filename.value.returned||!preflightNative(waterReceiver_))return false;
    if(!g.entered){
        if(ordinal)return false;
        g.entered=true;g.signature=signature;g.section=filename.section;g.count=source_.selected20;
        g.tableIdentity=filename.tableIdentity;g.dataIdentity=filename.dataIdentity;
    }else if(g.signature!=signature||g.completed!=ordinal)return false;
    const auto index=arrayScalarIndexNative(ordinal);const bool integer=arrayScalarIntegerNative(ordinal);
    if(g.destinations[ordinal]||(integer?g.integers[index].attempted:g.floats[index].attempted))return false;
    g.destinations[ordinal]=&destination;
    static constexpr std::array<std::string_view,14> names{{"positionX","positionY","positionZ","widthX","widthZ",
        "numFrames","velocityX","velocityZ","rotation","tileX","tileZ","tile","channel","PhysicsType"}};
    static constexpr std::array<std::uint32_t,14> defaults{{0u,0u,0u,0u,0u,0u,0u,0u,0u,0x3f800000u,0x3f800000u,1u,0u,0u}};
    static constexpr std::array<std::uint32_t,14> signatureDelays{{0x2b993cu,0x2b9954u,0x2b996cu,0x2b9984u,
        0x2b999cu,0x2b99b4u,0x2b99ccu,0x2b99e4u,0x2b99fcu,0x2b9a18u,0x2b9a34u,0x2b9a4cu,0x2b9a64u,0x2b9a7cu}};
    static constexpr std::array<std::uint32_t,3> poolDelays{{0x2b9cb8u,0x2b9cd0u,0x2b9ce8u}};
    g.pc=signature?signatureDelays[ordinal]:poolDelays[ordinal];
    struct Scope {bool& active;explicit Scope(bool& value)noexcept:active(value){active=true;}
        ~Scope(){active=false;}} scope(g.active);
    if(integer){
        auto& f=g.integers[index];f.attempted=true;g.pc=0x158b48;
        if(!findWaterField158240(f,names[ordinal]))return false;
        if(*f.found){
            if(!convertWaterInteger158920(f))return false;
            g.pc=*f.storePC;destination=f.converted;f.result=f.converted;f.innerReturned=true;
            g.pc=0x158a60;g.pc=0x158b5c;f.returned=true;
        }else{
            g.pc=0x158a60;f.innerReturned=false;
            g.pc=0x158b60;destination=defaults[ordinal];f.result=defaults[ordinal];f.returned=false;
        }
        g.pc=0x158b78;
    }else{
        auto& f=g.floats[index];f.attempted=true;g.pc=0x158b9c;
        if(!findWaterField158240(f,names[ordinal]))return false;
        if(*f.found){
            g.pc=0x158bac;if(!convertWaterDecimal302E10(f))return false;
            g.pc=0x158bc4;destination=f.converted;f.result=f.converted;f.returned=true;
        }else{
            g.pc=0x158bdc;destination=defaults[ordinal];f.result=defaults[ordinal];f.returned=false;
        }
        g.pc=0x158bf4;
    }
    ++g.completed;g.pc=(signature?signatureDelays[ordinal]:poolDelays[ordinal])+4u;
    return true; //No source parent cursor or process-global value is published.
}
bool OriginalWaterSourceConfig50LA::preflightArrayScalarsNative()const noexcept {
    const auto& g=nativeArrayScalars_;const auto& filename=nativeArrayFilename_;
    if(g.active||g.completed>(g.signature?14u:3u))return false;
    static constexpr std::array<std::uint32_t,10> floatDefaults{{0u,0u,0u,0u,0u,0u,0u,0u,0x3f800000u,0x3f800000u}};
    static constexpr std::array<std::uint32_t,4> intDefaults{{0u,1u,0u,0u}};
    for(std::size_t i=0;i<g.floats.size();++i)if(!preflightFloatValueNative(g.floats[i],g.count,floatDefaults[i]))return false;
    for(std::size_t i=0;i<g.integers.size();++i)if(!preflightIntegerValueNative(g.integers[i],g.count,intDefaults[i]))return false;
    if(!g.entered){
        if(g.signature||g.pc||g.count||g.completed||g.section||g.tableIdentity||g.dataIdentity)return false;
    }else if(!filename.value.returned||pc_!=0x30492cu||g.section!=filename.section||g.count!=filename.count||
        g.count!=source_.selected20||g.tableIdentity!=filename.tableIdentity||g.dataIdentity!=filename.dataIdentity||
        source_.child24!=g.tableIdentity)return false;
    static constexpr std::array<std::uint32_t,15> signatureNext{{0x2b9928u,0x2b9940u,0x2b9958u,0x2b9970u,
        0x2b9988u,0x2b99a0u,0x2b99b8u,0x2b99d0u,0x2b99e8u,0x2b9a00u,0x2b9a1cu,0x2b9a38u,0x2b9a50u,0x2b9a68u,0x2b9a80u}};
    static constexpr std::array<std::uint32_t,4> poolNext{{0x2b9ca4u,0x2b9cbcu,0x2b9cd4u,0x2b9cecu}};
    for(std::size_t ordinal=0;ordinal<14u;++ordinal){
        const auto index=arrayScalarIndexNative(ordinal);const bool integer=arrayScalarIntegerNative(ordinal);
        const bool attempted=integer?g.integers[index].attempted:g.floats[index].attempted;
        const auto returned=integer?g.integers[index].returned:g.floats[index].returned;
        if(!g.entered||ordinal>=(g.signature?14u:3u)){
            if(attempted||returned||g.destinations[ordinal])return false;continue;
        }
        if(ordinal<g.completed){if(!attempted||!returned||!g.destinations[ordinal])return false;}
        else if(ordinal==g.completed){
            if(returned||bool(g.destinations[ordinal])!=attempted)return false;
            if(!attempted&&g.pc!=(g.signature?signatureNext[ordinal]:poolNext[ordinal]))return false;
        }else if(attempted||returned||g.destinations[ordinal])return false;
    }
    return !g.entered||g.completed!=(g.signature?14u:3u)||g.pc==(g.signature?0x2b9a80u:0x2b9cecu);
}

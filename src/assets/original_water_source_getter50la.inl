//Twenty actual Water float-getter invocations, never a detached value parser.
bool OriginalWaterSourceConfig50LA::fieldKnownLength(const Text::Bytes& bytes,std::size_t start,
    std::size_t& length,std::uint32_t at)noexcept {
    auto& flowPC=fieldFlowPCNative();
    flowPC=at;length=0;
    for(std::size_t i=start;i<bytes.value.size();++i){
        if(!bytes.known[i])return false;if(!bytes.value[i]){length=i-start;return true;}
    }
    return false;
}
bool OriginalWaterSourceConfig50LA::fieldToken29FFA0(FieldLookup& f,bool first,
    std::optional<std::size_t>& returned)noexcept {
    auto& flowPC=fieldFlowPCNative();
    returned.reset();flowPC=0x29ffb0;if(!f.tokenNext)return true;
    const auto start=*f.tokenNext;f.tokenCurrent=start;
    const auto delimiter=[first](std::uint8_t b)noexcept{return b=='\n'||(first&&b=='=');};
    std::size_t split=start;
    for(;;++split){
        flowPC=0x29ffcc;if(split>=f.scratch.value.size()||!f.scratch.known[split])return false;
        const auto b=f.scratch.value[split];if(!b){f.tokenNext.reset();returned=start;return true;}
        if(delimiter(b))break;
    }
    f.tokenNext=split+1u;f.scratch.put(split,0); //29FFE4..EC, retained on any later refusal.
    flowPC=0x29fff4;
    if(*f.tokenNext>=f.scratch.value.size()||!f.scratch.known[*f.tokenNext])return false;
    if(!f.scratch.value[*f.tokenNext]){f.tokenNext.reset();returned=start;return true;}
    for(;;){
        flowPC=0x2a001c;
        if(*f.tokenNext>=f.scratch.value.size()||!f.scratch.known[*f.tokenNext])return false;
        const auto b=f.scratch.value[*f.tokenNext];
        //The original membership loop compares the delimiter's terminating
        //NUL too. Repeated delimiters can advance into old/unknown tail bytes.
        if(b&&!delimiter(b))break;++*f.tokenNext;
    }
    flowPC=0x2a0078;returned=start;return true;
}
bool OriginalWaterSourceConfig50LA::findWaterField158240(FieldLookup& f,std::string_view wanted,
    ShaderBytes* direct,ShaderCopy309230* copy,std::size_t directLimit)noexcept {
    auto& flowPC=fieldFlowPCNative();
    const bool filename=nativeArrayFilename_.active,scalars=nativeArrayScalars_.active;
    const bool current=filename||scalars,adapted=nativeWakeGetters_.active||current;
    const auto section=filename?nativeArrayFilename_.section:nativeArrayScalars_.section;
    const bool array=current&&section==ArrayFilenameSection::array;
    //Only the corresponding private scope can read current live native rows.
    //Completed historical General/Wake receipts never authorize a stale read.
    if(current){
        if((filename&&scalars)||nativeWakeGetters_.active||nativeClosed_||!borrowed_)return false;
        if(filename){
            const auto& g=nativeArrayFilename_;
            if(!g.entered||&f!=static_cast<const FieldLookup*>(&g.value)||!direct||direct!=g.destinationIdentity||
               copy!=&nativeArrayFilename_.value.lookupCopy||directLimit!=128u)return false;
        }else{
            const auto& g=nativeArrayScalars_;
            if(!g.entered||g.completed>=(g.signature?14u:3u)||direct||copy)return false;
            const auto index=arrayScalarIndexNative(g.completed);
            const FieldLookup* expected=arrayScalarIntegerNative(g.completed)?
                static_cast<const FieldLookup*>(&g.integers[index]):static_cast<const FieldLookup*>(&g.floats[index]);
            if(&f!=expected)return false;
        }
        if(array){
            if(!nativeArrayLoad_.nativeLoaded||!nativeArrayLoad_.table||!nativeArrayLoad_.data||
               source_.child24!=&arrayTableRole_||nativeArrayLoad_.pointerZeroRole!=&arrayDataRole_)return false;
        }else if(section!=ArrayFilenameSection::wake||nativeArraySelection_.returned!=false||
            nativeArrayRetirement_.attempted||nativeArrayLoad_.attempted||!nativeWakeLoad_.nativeLoaded||
            !nativeWakeLoad_.table||!nativeWakeLoad_.data||source_.child24!=&wakeTableRole_||
            nativeWakeLoad_.pointerZeroRole!=&wakeDataRole_)return false;
    }else if(adapted){
        if(!nativeWakeGetters_.entered||nativeWakeGetters_.completed>=5u||!nativeWakeLoad_.nativeLoaded||
           !nativeWakeLoad_.table||!nativeWakeLoad_.data||nativeClosed_||!borrowed_||direct||copy||
           source_.child24!=&wakeTableRole_||nativeWakeLoad_.pointerZeroRole!=&wakeDataRole_)return false;
    }else if(nativeWakeRetirement_.attempted)return false;
    const auto capacity=array?nativeArrayLoad_.capacity:adapted?nativeWakeLoad_.capacity:sectionCapacity_;
    const auto pointers=array?nativeArrayLoad_.pointersWritten:adapted?nativeWakeLoad_.pointersWritten:sectionPointersWritten_;
    const auto stored=array?nativeArrayLoad_.stored:adapted?nativeWakeLoad_.stored:sectionStored_;
    const auto* rows=array?nativeArrayLoad_.rows.get():adapted?nativeWakeLoad_.rows.get():sectionRows_.get();
    flowPC=0x158648;
    for(;f.row<source_.selected20&&!(source_.selected20&0x80000000u);++f.row){
        flowPC=0x1586d0;
        if(f.row>=capacity||f.row>=pointers||f.row>=stored||!rows)return false;
        const auto& row=rows[f.row];std::size_t rowLength=0;
        if(!fieldKnownLength(row,0,rowLength,0x1586d0))return false;
        //The source copy is capped1023. Section strcpy already bounded to
        //that size; only the known prefix is read. Earlier scratch tails stay.
        for(std::size_t i=0;i<rowLength;++i)f.scratch.put(i,row.value[i]);
        flowPC=0x15871c;f.scratch.put(rowLength,0);
        f.tokenCurrent=0;f.tokenNext=rowLength?std::optional<std::size_t>(0):std::nullopt;
        f.keyToken.reset();f.valueToken.reset();f.valueBegin.reset();f.valueLength.reset();
        if(!fieldToken29FFA0(f,true,f.keyToken))return false;
        flowPC=0x1587ac;if(!f.keyToken)return false; //Original strlen(NULL), not a missing-key receipt.
        if(!fieldKnownLength(f.scratch,*f.keyToken,f.keyLength,0x1587ac))return false;
        for(std::size_t i=0;i<f.keyLength;++i)f.key.put(i,f.scratch.value[*f.keyToken+i]);
        flowPC=0x1587e8;f.key.put(f.keyLength,0);
        if(!fieldToken29FFA0(f,false,f.valueToken))return false;
        if(!f.valueToken)continue; //158800 skips comparison.
        bool equal=false;flowPC=0x15880c;
        const std::string_view key(reinterpret_cast<const char*>(f.key.value.data()),f.keyLength);
        if(!compareName308F00(key,wanted,equal))return false;
        if(!equal)continue;
        auto begin=*f.valueToken;flowPC=0x15881c;
        if(begin>=f.scratch.value.size()||!f.scratch.known[begin])return false;
        if(f.scratch.value[begin]=='"')++begin;
        std::size_t length=0;if(!fieldKnownLength(f.scratch,begin,length,0x158838))return false;
        if(length&&f.scratch.value[begin+length-1u]=='"'){
            flowPC=0x158860;f.scratch.put(begin+length-1u,0);--length;
        }
        f.valueBegin=begin;f.valueLength=length;flowPC=0x1588b4;
        if(length>=f.output.value.size())return false;
        if(direct){
            //Native non-overlap bound, NOT the source's limit=-1 truncation.
            if(!copy||length>=direct->value.size()||length>=directLimit)return false;
            if(!copyShader309230(*copy,f.scratch,begin,length,shaderScratchReadQuantumNative(begin),*direct))return false;
        }
        //Existing numeric private scratch, or historical completed direct-copy receipt.
        for(std::size_t i=0;i<=length;++i)f.output.put(i,f.scratch.value[begin+i]);
        f.outputKnown=length+1u;f.found=true;f.lookupReturned=true;flowPC=0x158918;return true;
    }
    flowPC=0x1588e4;
    if(direct){
        if(!copy)return false;
        const auto empty=shaderLiteralNative(false);
        if(!copyShader309230(*copy,empty,0,0,8,*direct))return false;
    }
    f.output.put(0,0);f.outputKnown=1;f.found=false;
    f.lookupReturned=true;flowPC=0x158918;return true;
}
bool OriginalWaterSourceConfig50LA::convertWaterDecimal302E10(FloatGetter& f)noexcept {
    auto& flowPC=fieldFlowPCNative();
    flowPC=0x302e10;f.decimalAttempted=true;
    //Native admission to the proved allocation/errno-free309C18 fast path:
    //optional sign,1+ integer digits, optional1+ fraction digits, <=9 total,
    //<=6 fractional, no exponent, whitespace or suffix. Refusal is NOT a
    //source parse failure, default result, zero or output-store permission.
    if(!f.lookupReturned||f.found!=true||!f.outputKnown||f.outputKnown>f.output.value.size())return false;
    const auto size=f.outputKnown-1u;std::size_t at=0,digits=0,fraction=0;
    bool negative=false;
    if(at<size&&(f.output.value[at]=='-'||f.output.value[at]=='+')){negative=f.output.value[at]=='-';++at;}
    const auto start=at;std::uint32_t numerator=0,denominator=1;
    while(at<size&&f.output.value[at]>='0'&&f.output.value[at]<='9'){
        if(++digits>9u)return false;numerator=numerator*10u+f.output.value[at++]-'0';
    }
    if(at==start)return false;
    if(at<size&&f.output.value[at]=='.'){
        const auto fractionalStart=++at;
        while(at<size&&f.output.value[at]>='0'&&f.output.value[at]<='9'){
            if(++digits>9u||++fraction>6u)return false;
            numerator=numerator*10u+f.output.value[at++]-'0';denominator*=10u;
        }
        if(at==fractionalStart)return false;
    }
    if(at!=size)return false;
    f.decimalAdmitted=true;f.numerator=numerator;f.denominator=denominator;f.fractional=fraction;f.negative=negative;
    using G=OriginalConfigProcessGlobals50LA;
    flowPC=0x30ab28;
    if(!G::reentrancyCurrentNative())return false;
    f.reent=G::state().reent38544C;if(*f.reent!=G::StdioIdentity::reent385160)return false;
    if(start){
        flowPC=0x309c80;f.signTableReached=true;
        if(!G::state().waterDecimalTablesKnown)return false;
    }
    //309C18 postpones trailing fractional zeros. Its effective exponent is
    //negative only through the last nonzero fractional digit; pure zero never
    //reaches the power table. Keep original value rational n/d either way.
    auto reduced=numerator;auto effective=fraction;
    while(effective&&reduced%10u==0u){reduced/=10u;--effective;}
    if(numerator&&effective){
        flowPC=0x30a1a8;f.powerTableReached=true;
        if(!G::state().waterDecimalTablesKnown)return false;
    }
    std::uint64_t bits64=0;std::uint32_t bits32=0;
    if(numerator){
        //3101E8 is exact for the <=999999999 accumulator. Source30FF18
        //divides by an exact power10<=1e6 and rounds nearest-even to binary64.
        //Integer long division is the bounded value projection of that path,
        //not host atof/strtod, host double arithmetic or a rounding-mode probe.
        std::uint64_t n=numerator,d=denominator;int exponent=0;
        while(n<d){n*=2u;--exponent;}while(n>=d*2u){d*=2u;++exponent;}
        std::uint64_t remainder=n-d,significand=1;
        for(unsigned i=0;i<52u;++i){
            remainder*=2u;significand*=2u;if(remainder>=d){++significand;remainder-=d;}
        }
        if(remainder*2u>d||(remainder*2u==d&&(significand&1u)))++significand;
        if(significand==(std::uint64_t(1)<<53u)){significand>>=1u;++exponent;}
        bits64=(std::uint64_t(negative)<<63u)|(std::uint64_t(exponent+1023)<<52u)|
            (significand&((std::uint64_t(1)<<52u)-1u));
        //310440 sticky reduction ->311008 ->310498 normal nearest-even path.
        //Every admitted nonzero value is normal binary32; no subnormal/NaN,
        //overflow, errno or heap-dependent path is being elided as success.
        std::uint32_t m=static_cast<std::uint32_t>(significand>>22u);
        if(significand&((std::uint64_t(1)<<22u)-1u))m|=1u;
        if((m&127u)==64u){if(m&128u)m+=64u;}else m+=63u;
        if(m&0x80000000u){m>>=1u;++exponent;}
        bits32=(std::uint32_t(negative)<<31u)|(std::uint32_t(exponent+127)<<23u)|((m>>7u)&0x7fffffu);
    }
    //Negative sign uses source30FC08(0,value), not a sign-bit xor. Its exact
    //zero result is positive even for authored -0/-0.000000.
    f.decimal64=bits64;flowPC=0x310440;f.converted=bits32;return true;
}
bool OriginalWaterSourceConfig50LA::readWaterFloat158B80(std::size_t ordinal,
    std::optional<std::uint32_t>& destination,D& d)noexcept {
    d={};if(ordinal>=floatGetters_.size()||floatGetters_[ordinal].attempted||
       (ordinal&&!floatGetters_[ordinal-1u].returned)||(ordinal>=15u&&!integerGetters_[5].returned)||!sectionReturn_||nativeClosed_||!borrowed_||
       !preflightNative(waterReceiver_))return fail(d,E::phase,0x158b80);
    static constexpr std::array<std::string_view,20> names{{"WaterBaseHeight","WorldWidthHeightInFeet","WaterHeightRange","BaseOpacity","MaxOpacity",
        "ChannelScale0","ChannelScale1","ChannelScale2","ChannelScale3",
        "FramesPerSecond0","FramesPerSecond1","FramesPerSecond2","FramesPerSecond3","FramesPerSecond4","WakeScale",
        "PolygonScale","AdditionalFov","PoolDepth","WaterBaseSwellHeight","WaterBaseSwellPeriod"}};
    static constexpr std::array<std::uint32_t,20> defaults{{0x41a00000u,0x45100000u,0x42200000u,0x42000000u,0x42800000u,
        0u,0u,0u,0u,0u,0u,0u,0u,0u,0x41200000u,0x3f7ff972u,0x40800000u,0x42c80000u,0u,0x40a00000u}};
    auto& f=floatGetters_[ordinal];f.attempted=true;pc_=0x158b9c;
    if(!findWaterField158240(f,names[ordinal]))return false;
    if(*f.found){
        pc_=0x158bac;if(!convertWaterDecimal302E10(f))return false;
        pc_=0x158bc4;destination=f.converted;f.result=f.converted;f.returned=true;
    }else{
        pc_=0x158bdc;destination=defaults[ordinal];f.result=defaults[ordinal];f.returned=false;
    }
    pc_=0x158bf4;return true;
}
bool OriginalWaterSourceConfig50LA::preflightFloatValueNative(const FloatGetter& f,
    std::uint32_t rowCount,std::uint32_t defaultValue)const noexcept {
    if(!f.attempted){
        if(f.lookupReturned||f.decimalAttempted||f.decimalAdmitted||f.found||f.returned||f.result||f.converted||
           f.decimal64||f.reent||f.signTableReached||f.powerTableReached||f.numerator||f.denominator!=1u||
           f.fractional||f.negative||f.row||f.tokenNext||f.keyToken||f.valueToken||f.valueBegin||f.valueLength||
           f.tokenCurrent||f.keyLength||f.outputKnown)return false;
        return true;
    }
    if(f.row>rowCount||f.lookupReturned!=bool(f.found)||
       bool(f.returned)!=bool(f.result)||(f.returned&&f.returned!=f.found)||
       (f.decimalAttempted&&f.found!=true)||(f.decimalAdmitted&&!f.decimalAttempted)||
       (f.reent&&!f.decimalAdmitted)||bool(f.converted)!=bool(f.decimal64)||
       (f.converted&&(!f.reent||*f.reent!=OriginalConfigProcessGlobals50LA::StdioIdentity::reent385160)))return false;
    if(f.lookupReturned){
        if(!f.outputKnown||f.outputKnown>f.output.value.size())return false;
        for(std::size_t j=0;j<f.outputKnown;++j)if(!f.output.known[j])return false;
        if(f.output.value[f.outputKnown-1u])return false;
        if(!*f.found&&(f.outputKnown!=1u||f.valueBegin||f.valueLength||f.decimalAttempted))return false;
        if(*f.found&&(!f.valueBegin||!f.valueLength||*f.valueLength+1u!=f.outputKnown))return false;
    }else if(f.outputKnown)return false;
    if(f.returned&&(*f.returned?(f.result!=f.converted):(f.result!=defaultValue)))return false;
    if(f.fractional>6u||f.numerator>999999999u||f.denominator>1000000u)return false;
    if(f.decimalAdmitted){std::uint32_t denominator=1;for(std::size_t n=0;n<f.fractional;++n)denominator*=10u;
        if(f.denominator!=denominator)return false;
    }
    if((f.signTableReached||f.powerTableReached)&&!f.reent)return false;
    if(f.converted&&!f.numerator&&(*f.converted||*f.decimal64))return false;
    return true;
}
bool OriginalWaterSourceConfig50LA::preflightFloatGettersNative()const noexcept {
    static constexpr std::array<std::uint32_t,20> defaults{{0x41a00000u,0x45100000u,0x42200000u,0x42000000u,0x42800000u,
        0u,0u,0u,0u,0u,0u,0u,0u,0u,0x41200000u,0x3f7ff972u,0x40800000u,0x42c80000u,0u,0x40a00000u}};
    for(std::size_t i=0;i<floatGetters_.size();++i){const auto& f=floatGetters_[i];
        if(f.attempted&&(!sectionReturn_||!textReturned_||(i&&!floatGetters_[i-1u].returned)||
           (i>=15u&&!integerGetters_[5].returned)))return false;
        if(!preflightFloatValueNative(f,sectionCounted_,defaults[i]))return false;
    }
    return true;
}

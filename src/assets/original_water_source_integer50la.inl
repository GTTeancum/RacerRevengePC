//Actual ordered158B30 ->158920 occurrences, never host atoi or a float cast.
std::optional<std::size_t> OriginalWaterSourceConfig50LA::booleanCandidateNative(std::string_view value)noexcept {
    //Admission only, NOT a source comparison/result. Actual probes below must
    //still read the shared ASCII owner before any boolean store is reached.
    const auto lower=[](unsigned char c)noexcept {return c>='A'&&c<='Z'?c+32u:std::uint32_t(c);};
    if(value.empty())return {};
    switch(lower(static_cast<unsigned char>(value[0]))){
    case 't':return 0u;case 'f':return 1u;case 'y':return 4u;case 'n':return 5u;
    case 'o':
        if(value.size()==2u&&lower(static_cast<unsigned char>(value[1]))=='n')return 2u;
        if(value.size()==3u&&lower(static_cast<unsigned char>(value[1]))=='f'&&
           lower(static_cast<unsigned char>(value[2]))=='f')return 3u;
        return {};
    default:return {};
    }
}
bool OriginalWaterSourceConfig50LA::compareFirst309538(std::uint8_t byte,std::uint8_t literal,bool& equal)noexcept {
    auto& flowPC=fieldFlowPCNative();
    using G=OriginalConfigProcessGlobals50LA;const G::WaterSprintfAccess key;
    std::int32_t a=0,b=0;
    flowPC=0x309580;if(!G::waterFold308F00(key,byte,a,flowPC))return false;
    flowPC=0x309584;if(!G::waterFold308F00(key,literal,b,flowPC))return false;
    //n=1 reaches the final unsigned-byte folds on either match or mismatch.
    flowPC=0x3095f0;if(!G::waterFold308F00(key,byte,a,flowPC))return false;
    flowPC=0x3095f4;if(!G::waterFold308F00(key,literal,b,flowPC))return false;
    equal=a==b;flowPC=0x309610;return true;
}
bool OriginalWaterSourceConfig50LA::integerClassify30AC38(std::uint8_t byte,
    std::uint32_t& mask,std::uint32_t at)noexcept {
    auto& flowPC=fieldFlowPCNative();
    using G=OriginalConfigProcessGlobals50LA;const auto& s=G::state();flowPC=at;
    if(s.thread!=std::this_thread::get_id()||!s.streamLookupAsciiKnown||!s.waterIntegerCtypeKnown)return false;
    //Exact full3C6391 bytes reached by the numeric-only admission below.
    if(byte==0)mask=0x20u;
    else if(byte=='+'||byte=='-')mask=0x10u;
    else if(byte>='0'&&byte<='9')mask=0x04u;
    else return false;
    return true;
}
bool OriginalWaterSourceConfig50LA::convertWaterInteger158920(IntegerGetter& f)noexcept {
    auto& flowPC=fieldFlowPCNative();
    f.conversionAttempted=true;flowPC=0x158950;
    //Native admission BEFORE any source probe: all original ASCII boolean
    //branches, or the previous exact bounded numeric subset. Prefix T/F/Y/N
    //does not inspect a suffix; ON/OFF requires full equality. Unsupported
    //nonboolean numeric syntax still stops here, never returns a default.
    if(!f.lookupReturned||f.found!=true||!f.outputKnown)return false;
    const auto size=f.outputKnown-1u;
    const std::string_view value(reinterpret_cast<const char*>(f.output.value.data()),size);
    const auto candidate=booleanCandidateNative(value);
    std::size_t first=0;
    if(candidate)f.booleanAdmitted=true;
    else {
        if(size&&(f.output.value[0]=='+'||f.output.value[0]=='-'))first=1;
        if(first>=size||size-first>9u||f.output.value[first]<'0'||f.output.value[first]>'9'||
           (f.output.value[first]=='0'&&size-first!=1u))return false;
        for(std::size_t i=first;i<size;++i)if(f.output.value[i]<'0'||f.output.value[i]>'9')return false;
        f.numericAdmitted=true;f.admittedDigits=size-first;f.firstDigit=first;
        f.negative=first&&f.output.value[0]=='-';
    }
    static constexpr std::array<std::uint32_t,6> calls{{0x158950u,0x158978u,0x15899cu,0x1589c0u,0x1589e8u,0x158a10u}};
    static constexpr std::array<std::uint8_t,6> initials{{'T','F',0,0,'Y','N'}};
    for(std::size_t i=0;i<calls.size();++i){
        flowPC=calls[i];++f.probesEntered;bool equal=false;
        const bool returned=initials[i]?compareFirst309538(f.output.value[0],initials[i],equal):
            compareName308F00(value,i==2u?"ON":"OFF",equal);
        if(!returned)return false;++f.probesReturned;
        if(equal){
            if(!candidate||*candidate!=i)return false;
            static constexpr std::array<std::uint32_t,6> stores{{0x158968u,0x158990u,0x1589b4u,0x1589d8u,0x158a00u,0x158a28u}};
            f.booleanProbe=i;f.converted=(i%2u)?0u:1u;f.storePC=stores[i];flowPC=*f.storePC;
            return true; //Boolean branch never reaches30AE70 or its reent/tables.
        }
    }
    if(!f.numericAdmitted)return false;
    using G=OriginalConfigProcessGlobals50LA;
    flowPC=0x30ae84;if(!G::reentrancyCurrentNative())return false;
    f.reent=G::state().reent38544C;if(*f.reent!=G::StdioIdentity::reent385160)return false;
    std::uint32_t classification=0;
    if(!integerClassify30AC38(f.output.value[0],classification,0x30ac94))return false;
    f.whitespaceClassified=true;if(classification&8u)return false;
    //Literal0, including+0/-0, genuinely takes source base8. Other admitted
    //strings take base10. None takes0x, whitespace, partial parse or overflow.
    flowPC=0x30ad18;f.base=f.output.value[first]=='0'?8u:10u;
    f.limit=f.negative?0x8000000000000000ULL:0x7fffffffffffffffULL;
    //Original remainder30F260 and quotient30EC90 normalize base through two
    //distinct read-only tables. Entries+8/+10 are4 in BOTH actual tables.
    flowPC=0x30f384;f.remainderTableReached=true;
    if(!G::state().waterIntegerDivisionTablesKnown)return false;
    f.cutlim=static_cast<std::uint32_t>(*f.limit%f.base);
    flowPC=0x30ee74;f.quotientTableReached=true;
    if(!G::state().waterIntegerDivisionTablesKnown)return false;
    f.cutoff=*f.limit/f.base;
    for(std::size_t i=first;;++i){
        if(!integerClassify30AC38(f.output.value[i],classification,0x30adc0))return false;
        ++f.digitClassifications;
        if(!(classification&4u)){if(classification&3u)return false;break;}
        const auto digit=std::uint32_t(f.output.value[i]-'0');
        if(digit>=f.base||f.accumulator>*f.cutoff||
           (f.accumulator==*f.cutoff&&digit>*f.cutlim))return false;
        //Pure30EC30 multiply and DADDU, bounded well below signed64 limits.
        flowPC=0x30ada8;f.accumulator=f.accumulator*f.base+digit;++f.digitsConsumed;
    }
    const auto bits=f.negative?std::uint64_t(0)-f.accumulator:f.accumulator;
    flowPC=0x30ae38;f.endOffset=size; //Actual local end pointer, never an EE address guess.
    flowPC=0x30aea0;f.integer64=bits;
    flowPC=0x158a40;f.converted=static_cast<std::uint32_t>(bits);f.storePC=0x158a44u; //DSLL32/DSRA32 then SW low32.
    return true;
}
bool OriginalWaterSourceConfig50LA::readWaterInteger158B30(std::size_t ordinal,
    std::optional<std::uint32_t>& destination,D& d)noexcept {
    d={};if(ordinal>=integerGetters_.size()||integerGetters_[ordinal].attempted||
       !floatGetters_[14].returned||(ordinal>=6u&&!floatGetters_[19].returned)||(ordinal==11u&&!shaderGetter_.returned)||
       (ordinal&&!integerGetters_[ordinal-1u].returned)||
       !sectionReturn_||nativeClosed_||!borrowed_||!preflightNative(waterReceiver_))return fail(d,E::phase,0x158b30);
    static constexpr std::array<std::string_view,12> names{{"EnvColorRed","EnvColorGreen","EnvColorBlue",
        "FrothColorRed","FrothColorGreen","FrothColorBlue","RenderInfinitePlane",
        "EnableBaseDisplacement","LodBias","NumSignatureWaves","NumPools","NumZones"}};
    static constexpr std::array<std::uint32_t,12> defaults{{80,80,80,80,80,80,1,1,3,0,0,16}};
    auto& f=integerGetters_[ordinal];f.attempted=true;pc_=0x158b48;
    if(!findWaterField158240(f,names[ordinal]))return false;
    if(*f.found){
        if(!convertWaterInteger158920(f))return false;
        pc_=*f.storePC;destination=f.converted;f.result=f.converted;f.innerReturned=true;
        pc_=0x158a60;pc_=0x158b5c;f.returned=true;
    }else{
        pc_=0x158a60;f.innerReturned=false; //158920 missing does NOT write its supplied integer.
        pc_=0x158b60;destination=defaults[ordinal];f.result=defaults[ordinal];f.returned=false;
    }
    pc_=0x158b78;return true;
}
bool OriginalWaterSourceConfig50LA::preflightIntegerValueNative(const IntegerGetter& f,
    std::uint32_t rowCount,std::uint32_t defaultValue)const noexcept {
    if(!f.attempted){
        if(f.lookupReturned||f.found||f.row||f.tokenNext||f.keyToken||f.valueToken||f.valueBegin||f.valueLength||
           f.tokenCurrent||f.keyLength||f.outputKnown||f.conversionAttempted||f.numericAdmitted||f.booleanAdmitted||f.booleanProbe||f.storePC||f.innerReturned||
           f.returned||f.result||f.converted||f.cutlim||f.integer64||f.limit||f.cutoff||f.reent||f.endOffset||
           f.admittedDigits||f.firstDigit||f.digitsConsumed||f.digitClassifications||f.probesEntered||f.probesReturned||
           f.base||f.accumulator||f.negative||f.whitespaceClassified||f.remainderTableReached||f.quotientTableReached)return false;
        return true;
    }
    if(f.row>rowCount||f.lookupReturned!=bool(f.found)||bool(f.returned)!=bool(f.result)||
       bool(f.returned)!=bool(f.innerReturned)||(f.returned&&(f.returned!=f.found||f.innerReturned!=f.found))||
       (f.conversionAttempted&&f.found!=true)||((f.numericAdmitted||f.booleanAdmitted)&&!f.conversionAttempted)||
       (f.numericAdmitted&&f.booleanAdmitted)||bool(f.converted)!=bool(f.storePC))return false;
    if(f.lookupReturned){
        if(!f.outputKnown||f.outputKnown>f.output.value.size())return false;
        for(std::size_t j=0;j<f.outputKnown;++j)if(!f.output.known[j])return false;
        if(f.output.value[f.outputKnown-1u])return false;
        if(!*f.found&&(f.outputKnown!=1u||f.valueBegin||f.valueLength||f.conversionAttempted))return false;
        if(*f.found&&(!f.valueBegin||!f.valueLength||*f.valueLength+1u!=f.outputKnown))return false;
    }else if(f.outputKnown)return false;
    if(f.returned&&(*f.returned?f.result!=f.converted:f.result!=defaultValue))return false;
    if(!f.numericAdmitted&&!f.booleanAdmitted){
        if(f.admittedDigits||f.firstDigit||f.negative||f.probesEntered||f.probesReturned||f.reent||
           f.whitespaceClassified||f.base||f.limit||f.remainderTableReached||f.quotientTableReached||
           f.cutlim||f.cutoff||f.digitClassifications||f.digitsConsumed||f.accumulator||f.endOffset||f.integer64||f.converted||f.booleanProbe||f.storePC)return false;
        return true;
    }
    if(f.probesEntered>6u||f.probesReturned>f.probesEntered||f.probesEntered-f.probesReturned>1u)return false;
    if(f.booleanAdmitted){
        const std::string_view value(reinterpret_cast<const char*>(f.output.value.data()),f.outputKnown-1u);
        const auto candidate=booleanCandidateNative(value);
        if(!candidate||f.probesEntered>*candidate+1u||f.admittedDigits||f.firstDigit||f.digitsConsumed||
           f.digitClassifications||f.accumulator||f.negative||f.reent||f.whitespaceClassified||f.base||f.limit||
           f.remainderTableReached||f.quotientTableReached||f.cutlim||f.cutoff||f.endOffset||f.integer64)return false;
        if(f.booleanProbe){
            static constexpr std::array<std::uint32_t,6> stores{{0x158968u,0x158990u,0x1589b4u,0x1589d8u,0x158a00u,0x158a28u}};
            if(f.booleanProbe!=candidate||f.probesReturned!=*candidate+1u||f.probesEntered!=f.probesReturned||
               f.converted!=((*candidate%2u)?0u:1u)||f.storePC!=stores[*candidate])return false;
        }else if(f.converted||f.storePC||f.returned||f.probesReturned>*candidate)return false;
        return true;
    }
    if(f.booleanProbe||(f.storePC&&f.storePC!=0x158a44u))return false;
    if(!f.admittedDigits||f.admittedDigits>9u||f.firstDigit>1u||
       f.firstDigit+f.admittedDigits+1u!=f.outputKnown||f.probesEntered>6u||
       f.probesReturned>f.probesEntered||f.probesEntered-f.probesReturned>1u||
       (f.reent&&f.probesReturned!=6u)||
       (f.whitespaceClassified&&(!f.reent||*f.reent!=OriginalConfigProcessGlobals50LA::StdioIdentity::reent385160))||
       bool(f.base)!=bool(f.limit)||(f.base&&!f.whitespaceClassified)||
       (f.remainderTableReached&&!f.base)||(f.cutlim&&!f.remainderTableReached)||
       (f.quotientTableReached&&!f.cutlim)||(f.cutoff&&!f.quotientTableReached)||
       f.digitsConsumed>f.admittedDigits||f.accumulator>999999999u||
       f.digitClassifications<f.digitsConsumed||f.digitClassifications>f.digitsConsumed+1u||
       (f.digitClassifications&&!f.cutoff)||bool(f.integer64)!=bool(f.endOffset)||
       bool(f.integer64)!=bool(f.converted))return false;
    if(f.base&&(f.base!=(f.output.value[f.firstDigit]=='0'?8u:10u)||
       f.limit!=(f.negative?0x8000000000000000ULL:0x7fffffffffffffffULL)))return false;
    if(f.cutlim&&f.cutlim!=static_cast<std::uint32_t>(*f.limit%f.base))return false;
    if(f.cutoff&&f.cutoff!=*f.limit/f.base)return false;
    if(f.integer64){
        const auto bits=f.negative?std::uint64_t(0)-f.accumulator:f.accumulator;
        if(f.digitsConsumed!=f.admittedDigits||f.digitClassifications!=f.admittedDigits+1u||
           f.endOffset!=f.outputKnown-1u||f.integer64!=bits||f.converted!=static_cast<std::uint32_t>(bits))return false;
    }
    return true;
}
bool OriginalWaterSourceConfig50LA::preflightIntegerGettersNative()const noexcept {
    static constexpr std::array<std::uint32_t,12> defaults{{80,80,80,80,80,80,1,1,3,0,0,16}};
    for(std::size_t i=0;i<integerGetters_.size();++i){const auto& f=integerGetters_[i];
        if(f.attempted&&(!sectionReturn_||!floatGetters_[14].returned||(i>=6u&&!floatGetters_[19].returned)||
           (i==11u&&!shaderGetter_.returned)||(i&&!integerGetters_[i-1u].returned)))return false;
        if(!preflightIntegerValueNative(f,sectionCounted_,defaults[i]))return false;
    }
    return true;
}

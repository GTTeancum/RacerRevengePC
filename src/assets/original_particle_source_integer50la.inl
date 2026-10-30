// Private owned-field conversion, included inside OriginalParticleSourceConfig50LA.
// Input is ONLY the NUL-terminated, fully known prefix returned by158240. The
// string_view excludes its proved NUL; it is not an independently supplied receipt.
// No host strtol/locale/errno, floating parser, or synthesized unknown byte.
    bool convertInteger158920(std::string_view value,std::int32_t& output)noexcept {
        const auto byte=[&](std::size_t i)noexcept -> unsigned {
            return i<value.size()?static_cast<unsigned char>(value[i]):0u;
        };
        const auto lower=[](unsigned c)noexcept {return c>='A'&&c<='Z'?c+32u:c;};
        //309538(n=1) first uses signed LB/ctype; unsupported negative-table
        //indices stop before that read, even though its final compare is unsigned.
        if(byte(0)>=128){sourcePC_=0x309580;return false;}
        const unsigned first=lower(byte(0));
        if(first=='t'){output=1;sourcePC_=0x158968;return true;}
        if(first=='f'){output=0;sourcePC_=0x158990;return true;}
        //308F00 full case-insensitive comparisons, in original order. Stop at
        //first mismatch; do not inspect a later high-bit byte unnecessarily.
        const auto equals=[&](const char* literal,bool& same)noexcept {
            same=false;std::size_t i=0;
            for(;;++i){const unsigned a=byte(i),b=static_cast<unsigned char>(literal[i]);
                if(a>=128){sourcePC_=0x308F00;return false;}
                if(lower(a)!=lower(b))return true;
                if(!a){same=true;return true;}
            }
        };
        bool same=false;if(!equals("ON",same))return false;
        if(same){output=1;sourcePC_=0x1589B4;return true;}
        if(!equals("OFF",same))return false;
        if(same){output=0;sourcePC_=0x1589D8;return true;}
        if(first=='y'){output=1;sourcePC_=0x158A00;return true;}
        if(first=='n'){output=0;sourcePC_=0x158A28;return true;}
        //30AE70 ->30AC38, base=0, 64-bit long. Whitespace is ctype bit8:
        //authenticated bytes09..0D and20 only. Cursor s2 points AFTER c.
        std::size_t p=0;unsigned c=0;
        do {c=byte(p++);if(c>=128){sourcePC_=0x30AC94;return false;}}
        while((c>=9&&c<=13)||c==32);
        bool negative=false;
        if(c=='-'||c=='+'){negative=c=='-';c=byte(p++);}
        unsigned base=0;
        if(c=='0'&&(byte(p)=='x'||byte(p)=='X')){c=byte(p+1);p+=2;base=16;}
        if(!base)base=c=='0'?8u:10u;
        const std::uint64_t limit=negative?0x8000000000000000ULL:0x7fffffffffffffffULL;
        const std::uint64_t cutoff=limit/base;const unsigned cutlim=unsigned(limit%base);
        std::uint64_t magnitude=0;int any=0;
        for(;;){
            if(c>=128){sourcePC_=0x30ADC0;return false;}
            unsigned digit;
            if(c>='0'&&c<='9')digit=c-'0';
            else if(c>='A'&&c<='Z')digit=c-'A'+10;
            else if(c>='a'&&c<='z')digit=c-'a'+10;
            else break;
            if(digit>=base)break;
            if(any<0||magnitude>cutoff||(magnitude==cutoff&&digit>cutlim))any=-1;
            else {any=1;magnitude=magnitude*base+digit;}
            c=byte(p++);
        }
        //Overflow consumes all accepted digits first, then30AE18 writes ERANGE
        //34 through reentrancy object38544C. That owner is not joined: do NOT
        //manufacture its write, saturating return, end-pointer store, or output.
        if(any<0){sourcePC_=0x30AE18;return false;}
        const std::uint64_t bits=negative?std::uint64_t(0)-magnitude:magnitude;
        //30AE38 writes original start if no digit, else s2-1, into the actual
        //getter's stack-local end pointer. It is not consulted by158920.
        const std::size_t endOffset=any?p-1:0;(void)endOffset;
        const std::uint32_t low=static_cast<std::uint32_t>(bits);
        //158A3C/40 truncate+sign extend low32, then158A44 SW. Avoid native
        //implementation-defined unsigned->signed narrowing and signed overflow.
        output=low<=0x7fffffffU?static_cast<std::int32_t>(low):
            static_cast<std::int32_t>(std::int64_t(low)-0x100000000LL);
        sourcePC_=0x158A44;return true;
    }

std::uint32_t integerGetterCount_=0;
bool integerGetterBounded_=false;
bool readCount158B30(bool frames,std::int32_t& actualDestination,D& d)noexcept {
    d={};
    if(!textureMapReturned_||!textSectionForFieldCurrent()||integerGetterBounded_||
       integerGetterCount_!=(frames?0u:1u))return false;
    integerGetterBounded_=true;
    std::string_view value;bool found=false;
    if(!findGeneralInfoField158240(frames?GeneralInfoField::numFrames:
                                 GeneralInfoField::numGroups,value,found))return false;
    //Original158920 has a fresh stack20 string output.158240's copy is into
    //that local, not the particle integer. Only its proven prefix+NUL is used.
    //The lookup already bounded each string to1023 bytes, within its1024local.
    if(found){
        if(!convertInteger158920(value,actualDestination))return false;
    }else {
        //158920 returns0 without touching the supplied integer.158B60 then
        //writes the actual caller default0 into particle154 or140.
        sourcePC_=0x158B60u;actualDestination=0;
    }
    ++integerGetterCount_;integerGetterBounded_=false;
    sourcePC_=0x158B68u;return true; //Whether sourcev0 is0 or1, caller ignores it.
}

bool glowGetterAttempted_=false,glowGetterReturned_=false;
bool readGlowScalar158B80(float& actualDestination,D& d)noexcept {
    d={};if(integerGetterCount_!=2||integerGetterBounded_||glowGetterAttempted_||
            !textSectionForFieldCurrent())return false;
    glowGetterAttempted_=true;
    std::string_view value;bool found=false;
    if(!findGeneralInfoField158240(GeneralInfoField::glowScalar,value,found))return false;
    if(!found){sourcePC_=0x158BDCu;actualDestination=255.f;}
    else {
        sourcePC_=0x158BACu;
        //Explicit exact-literal native adapter, NOT a general302E10 parser.
        //Actual RA PRT says128.0, exactly2^7 in binary64 and binary32. Require
        //that SAME parsed token, not member hash/prewarm or constructor value.
        //No host strtof, rounding probe, clipping, or unknown-value fallback.
        if(value!="128.0")return false;
        sourcePC_=0x158BC4u;actualDestination=128.f;
    }
    glowGetterReturned_=true;sourcePC_=0x158BE4u;return true;
}
bool readGroupInteger158920(std::uint32_t ordinal,std::uint32_t lane,
    std::optional<std::int32_t>* actualDestination,std::uint8_t* actualStorage,D& d)noexcept {
    d={};if(!glowGetterReturned_||!groupSectionSelected_||ordinal+1!=nextGroupSection_||
       lane!=groupFieldOrdinal_||lane>=3||integerGetterBounded_||!textSectionForFieldCurrent())return false;
    integerGetterBounded_=true;
    const auto field=lane==0?GeneralInfoField::typeID:
        lane==1?GeneralInfoField::firstFrame:GeneralInfoField::lastFrame;
    std::string_view value;bool found=false;
    if(!findGeneralInfoField158240(field,value,found))return false;
    if(found){
        std::int32_t converted;
        if(!convertInteger158920(value,converted))return false;
        //Source getter stores only after conversion. A NULL groups allocation
        //does NOT prevent prior section lookup/parse and only bounds HERE.
        //Keep the conversion's actual store PC (158968/.../158A44).
        if(!actualDestination||!actualStorage)return false;
        const auto bits=static_cast<std::uint32_t>(converted);
        for(std::size_t i=0;i<4;++i)actualStorage[i]=static_cast<std::uint8_t>(bits>>(8*i));
        *actualDestination=converted;
    }
    //Unlike158B30 there is NO default store on missing158920. Unknown malloc
    //contents stay unknown. Caller ignores return0 and advances to next lane.
    ++groupFieldOrdinal_;integerGetterBounded_=false;sourcePC_=0x158A54u;return true;
}

//Explicit exact-integer native adapter for the actual unsigned-decimal UV
//tokens, NOT general302E10 strtod. Every integer through2^24 is exactly
//representable in binary32 and binary64. Unsupported syntax/range bounds at
//the original conversion call without output stores. No host locale/strtof.
bool exactUnsignedUV302E10(std::string_view value,float& output)noexcept {
    sourcePC_=0x158A94u;if(value.empty())return false;
    constexpr std::uint32_t limit=0x01000000u;
    std::uint32_t magnitude=0;
    for(const unsigned char c:value){
        if(c<'0'||c>'9')return false;
        const auto digit=static_cast<std::uint32_t>(c-'0');
        if(magnitude>(limit-digit)/10u)return false;
        magnitude=magnitude*10u+digit;
    }
    output=static_cast<float>(magnitude);return true;
}
bool readFrameUV158A70(std::uint32_t ordinal,std::uint32_t lane,D& d)noexcept {
    d={};if(!frameSectionSelected_||ordinal+1!=nextFrameSection_||
       lane!=frameFieldOrdinal_||lane>=4||frameUVWritten_!=4||
       frameGetterBounded_||!textSectionForFieldCurrent())return false;
    frameGetterBounded_=true;
    const auto field=lane==0?GeneralInfoField::uvLeft:lane==1?GeneralInfoField::uvRight:
        lane==2?GeneralInfoField::uvTop:GeneralInfoField::uvBottom;
    std::string_view value;bool found=false;
    if(!findGeneralInfoField158240(field,value,found))return false;
    if(found){
        float converted;
        if(!exactUnsignedUV302E10(value,converted))return false;
        sourcePC_=0x158AACu;frameUV770_[lane]=converted;
    }
    //Missing158A70 returns0 WITHOUT output store; the original zero local stays.
    ++frameFieldOrdinal_;frameGetterBounded_=false;sourcePC_=0x158AB4u;return true;
}

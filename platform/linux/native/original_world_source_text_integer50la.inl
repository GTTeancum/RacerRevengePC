// Private owned-field conversion, included inside OriginalWorldSourceConfig50LA.
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

//Included inside the actual World namespace. This owns result words/local
//configured-model effect receipts, never a live FCR31 or hardware-ready flag.
bool Owner::preflightSourceWaterProviderNumericNative()const noexcept {
    const auto& n=sourceWaterProviderNumeric_;const auto& m=sourceWaterProviderMembers_;
    const auto& h=sourceWaterProvider274720_;
    if(!n.entered){
        if(n.pc!=0x2b84c0u||n.thirdDivideBoundary||n.negated390||n.negateClearMask)return false;
        for(const auto& c:n.conversions)if(c.rawWord||c.bits)return false;
        for(std::size_t i=0;i<2;++i)if(n.quotients[i]||n.storedFields[i]||n.divideAssertedD[i]||
            n.divideClearMasks[i]||n.processWords[i]||n.processPublications[i])return false;
        return true;
    }
    if(!m.integerPrefixComplete||!m.field384||!m.fieldD8||!h.countAcquired||
       m.rawF1!=0xc0000000u||m.rawF0!=0x40000000u||h.pc!=n.pc)return false;
    const auto c=*m.field384,b=*m.fieldD8,b17=(b<<4u)+b;
    const std::uint32_t words[5]={c-1u,c-2u,c+2u,b17,b17+2u};
    for(std::size_t i=0;i<5;++i){const auto& conversion=n.conversions[i];
        if(conversion.rawWord&&*conversion.rawWord!=words[i])return false;
        if(conversion.bits){std::uint32_t expected=0;
            if(!conversion.rawWord||!g::detail::sourceWaterSmallWordFloat50LA(*conversion.rawWord,expected)||
               *conversion.bits!=expected)return false;
        }
    }
    if((n.conversions[1].rawWord&&!n.storedFields[0])||
       (n.conversions[2].rawWord&&!n.processWords[0])||
       (n.conversions[3].rawWord&&!n.processWords[1])||
       bool(n.conversions[3].rawWord)!=bool(n.conversions[4].rawWord)||
       (n.conversions[4].bits&&!n.conversions[3].bits))return false;
    const std::uint32_t numerators[2]={0xc0000000u,0x40000000u};
    for(std::size_t i=0;i<2;++i){
        if(n.quotients[i]!=n.storedFields[i]||bool(n.quotients[i])!=bool(n.divideAssertedD[i])||
           bool(n.quotients[i])!=bool(n.divideClearMasks[i]))return false;
        if(n.quotients[i]){std::uint32_t expected=0;bool asserted=false;
            if(!n.conversions[i].bits||!g::detail::sourceWaterTwoDivision50LA(numerators[i],
               *n.conversions[i].bits,expected,asserted)||*n.quotients[i]!=expected||
               *n.divideAssertedD[i]!=asserted||asserted||n.divideClearMasks[i]!=0x8000u)return false;
        }
    }
    if(bool(n.negated390)!=bool(n.negateClearMask)||bool(n.negated390)!=bool(n.processWords[0]))return false;
    if(n.negated390&&(!n.storedFields[1]||*n.negated390!=(*n.storedFields[1]^0x80000000u)||
       n.negateClearMask!=0xc000u||n.processWords[0]!=n.negated390))return false;
    if(n.processWords[1]&&(!n.processWords[0]||n.processWords[1]!=n.conversions[2].bits))return false;
    for(std::size_t i=0;i<2;++i)
        if(bool(n.processWords[i])!=bool(n.processPublications[i])||
           (n.processPublications[i]&&!*n.processPublications[i]))return false;
    //No fallible step separates each successful store from the next raw
    //operand preparation. A partial prefix may stop only at the value helper.
    if((n.quotients[0]&&!n.conversions[1].rawWord)||
       (n.quotients[1]&&(!n.processWords[0]||!n.conversions[2].rawWord))||
       (n.conversions[2].bits&&(!n.processWords[1]||!n.conversions[3].rawWord)))return false;
    //Every fallible value operation retains its raw operand and exact PC.
    const auto expectedPC=!n.conversions[0].rawWord?0x2b84c0u:
        !n.conversions[0].bits?0x2b84d4u:!n.quotients[0]?0x2b84d8u:
        !n.conversions[1].bits?0x2b84f4u:!n.quotients[1]?0x2b84f8u:
        !n.conversions[2].bits?0x2b8524u:!n.conversions[3].bits?0x2b8548u:
        !n.conversions[4].bits?0x2b854cu:0x2b8550u;
    if(n.pc!=expectedPC||n.thirdDivideBoundary!=(expectedPC==0x2b8550u))return false;
    if(h.nativeWithdrawn)return true;
    if(!worldApplication_)return false;const auto& p=worldApplication_->sourceWaterProcessScalars_;
    for(std::size_t i=0;i<2;++i)if(n.processPublications[i]){
        if(p.publications[i]<*n.processPublications[i])return false;
        if(!p.unknown&&p.publications[i]==*n.processPublications[i]&&p.words[i]!=n.processWords[i])return false;
    }
    return true;
}
bool Owner::continueSourceWaterProviderNumeric2B84C0(std::uint32_t& pc)noexcept {
    auto& n=sourceWaterProviderNumeric_;auto& m=sourceWaterProviderMembers_;auto& h=sourceWaterProvider274720_;
    if(pc!=0x2b84c0u||h.pc!=pc||sourcePostParticleModelsPC_!=pc||n.entered||!m.integerPrefixComplete||
       !sourceWaterProvider_||!sourceLoadApplication_||!worldApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||nativeScbThread_!=std::this_thread::get_id()||
       !preflightSourcePostParticleModelsNative()||!preflightSourceWorldParticleCountNative())return false;
    const auto at=[&](std::uint32_t p)noexcept {pc=sourcePostParticleModelsPC_=h.pc=n.pc=p;};
    n.entered=true;at(0x2b84c0);
    if(!worldApplication_->waterConstructorScalarsAdmittedNative())return false;
    auto& process=worldApplication_->sourceWaterProcessScalars_;
    const auto convert=[&](std::size_t i,std::uint32_t atPC)noexcept {
        at(atPC);std::uint32_t result=0;
        if(!g::detail::sourceWaterSmallWordFloat50LA(*n.conversions[i].rawWord,result))return false;
        n.conversions[i].bits=result;return true;
    };
    const auto divide=[&](std::size_t i,std::uint32_t numerator,std::uint32_t atPC)noexcept {
        at(atPC);std::uint32_t result=0;bool asserted=false;
        if(!g::detail::sourceWaterTwoDivision50LA(numerator,*n.conversions[i].bits,result,asserted))return false;
        n.quotients[i]=result;n.divideAssertedD[i]=asserted;n.divideClearMasks[i]=0x8000u;return true;
    };
    at(0x2b84c4);const auto firstC=*m.field384;
    at(0x2b84c8);const auto firstWord=firstC-1u;
    at(0x2b84cc);n.conversions[0].rawWord=firstWord;at(0x2b84d0);
    if(!convert(0,0x2b84d4)||!divide(0,*m.rawF1,0x2b84d8))return false;
    at(0x2b84dc);n.storedFields[0]=n.quotients[0];
    at(0x2b84e0);at(0x2b84e4);const auto secondC=*m.field384;
    at(0x2b84e8);const auto secondWord=secondC-2u;
    at(0x2b84ec);n.conversions[1].rawWord=secondWord;at(0x2b84f0);
    if(!convert(1,0x2b84f4)||!divide(1,*m.rawF0,0x2b84f8))return false;
    at(0x2b84fc);n.storedFields[1]=n.quotients[1];
    at(0x2b8500);at(0x2b8504);const auto read390=*n.storedFields[1];
    at(0x2b8508);n.negated390=read390^0x80000000u;n.negateClearMask=0xc000u;
    at(0x2b850c);process.words[0]=n.negated390;
    n.processWords[0]=process.words[0];n.processPublications[0]=++process.publications[0];
    at(0x2b8510);const auto globalC=*m.field384;at(0x2b8514);
    at(0x2b8518);const auto plusTwo=globalC+2u;
    at(0x2b851c);n.conversions[2].rawWord=plusTwo;at(0x2b8520);
    if(!convert(2,0x2b8524))return false;
    at(0x2b8528);process.words[1]=n.conversions[2].bits;
    n.processWords[1]=process.words[1];n.processPublications[1]=++process.publications[1];
    at(0x2b852c);const auto bands=*m.fieldD8;
    at(0x2b8530);const auto sixteen=bands<<4u;
    at(0x2b8534);const auto denominator=sixteen+bands;
    at(0x2b8538);const auto numerator=denominator+2u;
    at(0x2b853c);n.conversions[3].rawWord=denominator;
    at(0x2b8540);n.conversions[4].rawWord=numerator;at(0x2b8544);
    if(!convert(3,0x2b8548)||!convert(4,0x2b854c))return false;
    //This numerator is not +/-2. Do not reuse the bounded helper, perform
    //host division, write498 or claim derived return/World400/hardware events.
    at(0x2b8550);n.thirdDivideBoundary=true;return false;
}

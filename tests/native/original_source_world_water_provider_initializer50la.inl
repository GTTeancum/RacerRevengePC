//Included inside the actual World namespace. The returned source identity is
//the counted Entity, not its native wrapper. No initializer return is admitted.
bool Owner::preflightSourceWaterProviderInitializerNative()const noexcept {
    if(!preflightSourceWaterConfigNative())return false;
    const auto& r=sourceWaterProviderInitializer_;const auto& n=sourceWaterProviderNumeric_;
    const auto& h=sourceWaterProvider274720_;const auto& f=sourceWaterFactory274570_;
    if(!r.entered){
        if(r.pc!=0x2b8550u||r.ratio||r.field498||r.ratioClearMask||r.ratioAssertedD||r.constructorThis||
           r.constructorRestoreMask||r.constructorStackReleased||r.constructorReturned||r.world400Published||
           r.world400NativeWithdrawn||r.world400Read||r.lightPin||r.lightArgument||r.lightArgumentRead||
           r.filenameArgument||r.receiverValidated||r.initializerEntered||r.argumentA3||r.argumentT0||r.argumentF12||
           r.guardBefore||r.assertionWordStored||r.assertionGuardStored||r.assertionRead||r.assertionReturn||
           r.assertionCalled||r.baseEntered||r.field38||r.field38Stored||r.field4C||r.fieldB4||r.fieldB0||
           r.halfRaw||r.convertedB0||r.product||r.fieldB8||r.multiplyClearMask||r.forwardReadBytes||
           r.suffixIndex||r.extensionIsNull||r.extensionSelected||r.comparisonEntered||r.comparisonBytes||
           r.comparisonV0||r.comparisonReturnFoldEntered||r.comparisonFinalA||r.comparisonFinalB||r.wttLoaderEntered||r.alternateLoaderEntered||r.alternateLoaderReturn||
           r.filename398Known||r.filenameCopyReturned)return false;
        for(const auto& word:r.tailZeroWords)if(word)return false;return true;
    }
    if(!n.thirdDivideBoundary||!n.conversions[3].bits||!n.conversions[4].bits||!h.countAcquired||
       h.pc!=r.pc||n.pc!=r.pc)return false;
    if(r.ratio!=r.field498||bool(r.ratio)!=bool(r.ratioAssertedD)||bool(r.ratio)!=bool(r.ratioClearMask)||
       bool(r.ratio)!=r.constructorReturned||r.constructorReturned!=r.world400Published||
       r.world400Published!=r.world400Read||bool(r.constructorThis)!=r.constructorReturned||
       r.constructorStackReleased!=r.constructorReturned||
       r.constructorRestoreMask!=(r.constructorReturned?7u:0u))return false;
    if(r.ratio){std::uint32_t expected=0;bool asserted=false;
        if(!g::detail::sourceWaterBandRatio50LA(*n.conversions[4].bits,*n.conversions[3].bits,expected,asserted)||
           *r.ratio!=expected||*r.ratioAssertedD!=asserted||asserted||r.ratioClearMask!=0x8000u)return false;
    }
    for(const auto& word:r.tailZeroWords)
        if(word!=(r.constructorReturned?std::optional<std::uint32_t>(0):std::nullopt))return false;
    if((r.world400NativeWithdrawn&&!r.world400Published)||
       (r.lightArgumentRead&&!r.world400Read)||bool(r.lightArgument)!=r.lightArgumentRead||
       r.filenameArgument!=r.lightArgumentRead||r.receiverValidated!=r.lightArgumentRead||
       r.initializerEntered!=r.receiverValidated||bool(r.argumentA3)!=r.lightArgumentRead||
       bool(r.argumentT0)!=r.lightArgumentRead||bool(r.argumentF12)!=r.lightArgumentRead)return false;
    if(r.lightArgumentRead&&(!f.arguments||r.argumentA3!=sourcePostSkyTail_.water[0].argumentA2||
       r.argumentT0!=f.arguments->initializerT0||r.argumentF12!=f.arguments->initializerF12Bits))return false;
    if(r.guardBefore&&(!r.initializerEntered||*r.guardBefore>1u))return false;
    if(r.assertionWordStored!=(r.guardBefore&&!*r.guardBefore)||r.assertionGuardStored!=r.assertionWordStored||
       bool(r.assertionRead)!=bool(r.guardBefore)||r.assertionCalled!=bool(r.guardBefore)||
       bool(r.assertionReturn)!=r.assertionCalled||r.baseEntered!=r.assertionCalled)return false;
    if(r.assertionCalled&&(r.assertionRead!=1u||r.assertionReturn!=0u))return false;
    if(r.field38Stored!=r.baseEntered||bool(r.field38)!=r.field38Stored||bool(r.field4C)!=r.baseEntered||
       bool(r.fieldB4)!=r.baseEntered||bool(r.fieldB0)!=r.baseEntered||bool(r.halfRaw)!=r.baseEntered)return false;
    if(r.baseEntered&&(r.field38!=r.lightArgument||r.field4C!=r.argumentA3||
       r.fieldB4!=(*r.argumentT0>>3u)||r.fieldB0!=(*r.fieldB4<<3u)||r.halfRaw!=0x3f000000u))return false;
    if(r.convertedB0){std::uint32_t expected=0;
        if(!r.fieldB0||!g::detail::sourceWaterSmallWordFloat50LA(*r.fieldB0,expected)||*r.convertedB0!=expected)return false;
    }
    if(r.product!=r.fieldB8||bool(r.product)!=bool(r.multiplyClearMask))return false;
    if(r.product){std::uint32_t expected=0;
        if(!r.convertedB0||!r.halfRaw||!g::detail::sourceParticleFogMultiply50LA(*r.halfRaw,*r.convertedB0,expected)||
           r.product!=expected||r.multiplyClearMask!=0x8000u)return false;
    }
    if(bool(r.product)!=r.extensionSelected||r.extensionSelected!=bool(r.suffixIndex)||
       r.extensionSelected!=r.comparisonEntered||(r.extensionIsNull&&!r.extensionSelected))return false;
    if(r.extensionSelected){
        if(!f.formatted||!f.pathLength||*f.pathLength>=f.path70.size()||r.forwardReadBytes!=*f.pathLength+1u)return false;
        std::size_t last=*f.pathLength;while(last&&f.path70[last]!='.')--last;
        if(*r.suffixIndex!=last||r.extensionIsNull!=(f.path70[last]!='.'))return false;
    }else if(r.forwardReadBytes||r.comparisonBytes||r.comparisonV0)return false;
    if(r.extensionIsNull&&(r.comparisonBytes||r.comparisonV0))return false;
    bool comparisonEnded=false;std::int32_t finalA=0,finalB=0;
    if(r.comparisonBytes||r.comparisonV0){
        if(!r.suffixIndex||r.extensionIsNull||!f.pathLength)return false;
        constexpr char literal[]=".wtt";
        const auto fold=[](unsigned char byte)noexcept{return byte>='A'&&byte<='Z'?std::int32_t(byte)+32:std::int32_t(byte);};
        for(std::size_t i=0;i<r.comparisonBytes;++i){
            if(i>=sizeof(literal)||*r.suffixIndex+i>*f.pathLength||comparisonEnded)return false;
            const auto a=static_cast<unsigned char>(f.path70[*r.suffixIndex+i]);
            const auto b=static_cast<unsigned char>(literal[i]);if(a>127u)return false;
            finalA=fold(a);finalB=fold(b);comparisonEnded=!a||finalA!=finalB;
        }
    }
    if(r.comparisonReturnFoldEntered!=comparisonEnded||
       (r.comparisonFinalA&&(!comparisonEnded||*r.comparisonFinalA!=finalA))||
       (r.comparisonFinalB&&(!r.comparisonFinalA||*r.comparisonFinalB!=finalB)))return false;
    if(r.comparisonV0&&(!r.comparisonFinalA||!r.comparisonFinalB||*r.comparisonV0!=finalA-finalB))return false;
    if(comparisonEnded&&!r.comparisonV0){
        if(r.pc==0x308f90u){if(r.comparisonFinalA||r.comparisonFinalB)return false;}
        else if(r.pc==0x308f94u){if(!r.comparisonFinalA||r.comparisonFinalB)return false;}
        else return false;
    }
    if(r.wttLoaderEntered!=(r.comparisonV0&&!*r.comparisonV0)||
       r.alternateLoaderEntered!=(r.comparisonV0&&*r.comparisonV0!=0)||
       bool(r.alternateLoaderReturn)!=r.alternateLoaderEntered||
       (r.alternateLoaderReturn&&*r.alternateLoaderReturn!=0u)||
       (r.filenameCopyReturned&&!r.wttLoaderEntered))return false;
    if(r.filenameCopyReturned){
        if(!f.pathLength||*f.pathLength>=r.filename398.size()||r.filename398Known!=*f.pathLength+1u||
           std::memcmp(r.filename398.data(),f.path70.data(),r.filename398Known))return false;
    }else if(r.filename398Known)return false;
    //Only these authentic dependency/native-capacity stops are reachable.
    if(!r.ratio){if(r.pc!=0x2b8550u)return false;}
    else if(!r.lightArgumentRead){if(r.pc!=0x274750u)return false;}
    else if(!r.guardBefore){if(r.pc!=0x2b8680u)return false;}
    else if(!r.convertedB0){if(r.pc!=0x2b0bd4u)return false;}
    else if(!r.product){if(r.pc!=0x2b0bd8u)return false;}
    else if(r.extensionIsNull){if(r.pc!=0x308f08u)return false;}
    else if(!r.comparisonV0){if(r.pc!=0x308f34u&&r.pc!=0x308f3cu&&r.pc!=0x308f90u&&r.pc!=0x308f94u)return false;}
    else if(r.alternateLoaderEntered){if(r.pc!=0x2b0ca8u)return false;}
    else if(r.filenameCopyReturned){if(!sourceWaterConfig_.constructAttempted&&r.pc!=0x2b9314u)return false;}
    else if(r.pc!=0x309230u||!f.pathLength||*f.pathLength<r.filename398.size())return false;
    if(r.world400NativeWithdrawn){if(worldWater400_||r.lightPin)return false;}
    if(h.nativeWithdrawn)return !r.lightPin&&(!r.world400Published||r.world400NativeWithdrawn);
    if(r.constructorThis&&(!sourceWaterProvider_||r.constructorThis!=&sourceWaterProvider_->entity))return false;
    if(r.world400Published&&!r.world400NativeWithdrawn&&(!worldWater400_||*worldWater400_!=r.constructorThis))return false;
    if(r.world400NativeWithdrawn)return true;
    if(bool(r.lightPin)!=r.lightArgumentRead)return false;
    if(r.lightPin){const auto& pin=r.lightPin;
        if(!sourceLoadApplication_||!worldApplication_||pin->issuer_!=sourceLoadApplication_||
           pin->entities_!=worldApplication_||!pin->constructed_||pin->retired_||!pin->sourceIdentity()||
           r.lightArgument!=&pin->entity_||pin!=sourceTopologyLightManager_||
           pin!=sourceLoadApplication_->sourceLightManager426F60Owner_||
           pin.owner_before(sourceLoadApplication_->sourceLightManager426F60Owner_)||
           sourceLoadApplication_->sourceLightManager426F60Owner_.owner_before(pin))return false;
    }
    return true;
}
bool Owner::continueSourceWaterProviderInitializer2B8550(std::uint32_t& pc)noexcept {
    auto& r=sourceWaterProviderInitializer_;auto& n=sourceWaterProviderNumeric_;auto& h=sourceWaterProvider274720_;
    const auto& f=sourceWaterFactory274570_;
    if(pc!=0x2b8550u||n.pc!=pc||h.pc!=pc||sourcePostParticleModelsPC_!=pc||r.entered||
       !n.thirdDivideBoundary||!sourceWaterProvider_||!sourceLoadApplication_||!worldApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||nativeScbThread_!=std::this_thread::get_id()||
       !preflightSourcePostParticleModelsNative()||!preflightSourceWorldParticleCountNative())return false;
    const auto at=[&](std::uint32_t p)noexcept {pc=sourcePostParticleModelsPC_=h.pc=n.pc=r.pc=p;};
    r.entered=true;at(0x2b8550);
    //Native alias admission. Never overwrite a foreign/unknown old World400
    //or turn a backing failure into the original NULL allocation path.
    if(!worldWater400_||*worldWater400_||!f.arguments||!f.pathLength)return false;
    std::uint32_t ratio=0;bool asserted=false;
    if(!g::detail::sourceWaterBandRatio50LA(*n.conversions[4].bits,*n.conversions[3].bits,ratio,asserted))return false;
    r.ratio=ratio;r.ratioAssertedD=asserted;r.ratioClearMask=0x8000u;
    at(0x2b8554);r.field498=ratio;
    at(0x2b8558);at(0x2b855c);r.tailZeroWords[0]=0;
    at(0x2b8560);at(0x2b8564);r.tailZeroWords[1]=0;
    at(0x2b8568);at(0x2b856c);r.tailZeroWords[2]=0;
    at(0x2b8570);r.constructorThis=&sourceWaterProvider_->entity;
    at(0x2b8574);r.constructorRestoreMask|=1;at(0x2b8578);r.constructorRestoreMask|=2;
    at(0x2b857c);r.constructorRestoreMask|=4;at(0x2b8580);at(0x2b8584);
    r.constructorStackReleased=true;r.constructorReturned=true;
    at(0x274744);worldWater400_=r.constructorThis;r.world400Published=true;
    at(0x274748);r.world400Read=true;at(0x27474c);at(0x274750);
    const auto pin=sourceLoadApplication_->sourceLightManager426F60Owner_;
    //Pin the real wrapper, but carry its logical Entity as source426F60.
    //sourceMutationAvailable would incorrectly reject the existing RA zones.
    if(!pin||pin!=sourceTopologyLightManager_||pin->issuer_!=sourceLoadApplication_||
       pin->entities_!=worldApplication_||!pin->constructed_||pin->retired_||!pin->sourceIdentity()||
       worldApplication_->lightManager426F60_!=&pin->entity_)return false;
    r.lightPin=pin;r.lightArgument=worldApplication_->lightManager426F60_;r.lightArgumentRead=true;
    at(0x274754);r.filenameArgument=true;at(0x274758);r.argumentF12=f.arguments->initializerF12Bits;
    at(0x27475c);r.argumentA3=sourcePostSkyTail_.water[0].argumentA2;
    at(0x274760);at(0x274764);r.receiverValidated=true; //Owned vtable3D46E0+74=2B8660.
    at(0x274768);at(0x27476c);r.argumentT0=f.arguments->initializerT0;
    at(0x2b8660);r.initializerEntered=true;at(0x2b8680);
    auto& globals=worldApplication_->sourceWaterInitializerGlobals_;
    if(!globals.known||globals.guard3D66F0>1u||globals.assertion443EE8>1u||
       globals.guard3D66F0!=globals.assertion443EE8)return false;
    r.guardBefore=globals.guard3D66F0;
    at(0x2b8684);at(0x2b8688);at(0x2b868c);at(0x2b8690);at(0x2b8694);at(0x2b8698);at(0x2b869c);
    if(!*r.guardBefore){
        at(0x2b86a0);at(0x2b86a4);at(0x2b86a8);globals.assertion443EE8=1;r.assertionWordStored=true;
        at(0x2b86ac);globals.guard3D66F0=1;r.assertionGuardStored=true;
    }
    at(0x2b86b0);at(0x2b86b4);r.assertionRead=globals.assertion443EE8;
    at(0x2b86b8);at(0x2b86bc);at(0x2b86c0);at(0x2b86c4);at(0x2b86c8);
    at(0x2b86cc);at(0x2b86d0);r.assertionCalled=true;at(0x133660);
    //a0 is literal1. BOTH values of unjoined debug36C100 return0 without
    //diagnostics/callbacks. Do not invent which debug branch was taken.
    at(0x1336a8);at(0x1336ac);at(0x1336b0);r.assertionReturn=0;
    at(0x2b86d4);at(0x2b86d8);at(0x2b8700);at(0x2b8704);at(0x2b8708);at(0x2b870c);at(0x2b8710);
    at(0x2b8714);at(0x2b8718);at(0x2b0b70);r.baseEntered=true;
    at(0x2b0b8c);r.field38=r.lightArgument;r.field38Stored=true;
    at(0x2b0b90);at(0x2b0b94);at(0x2b0b98);at(0x2b0b9c);r.field4C=r.argumentA3;
    at(0x2b0ba0);at(0x2b0ba4);at(0x2b0ba8); //Authenticated T0 is positive120 or90.
    const auto eighth=*r.argumentT0>>3u;at(0x2b0bb4);r.fieldB4=eighth;
    at(0x2b0bb8);const auto readB4=*r.fieldB4;at(0x2b0bbc);at(0x2b0bc0);r.halfRaw=0x3f000000u;
    at(0x2b0bc4);at(0x2b0bc8);const auto timesEight=readB4<<3u;
    at(0x2b0bcc);r.fieldB0=timesEight;at(0x2b0bd0);at(0x2b0bd4);
    std::uint32_t converted=0;
    if(!g::detail::sourceWaterSmallWordFloat50LA(*r.fieldB0,converted))return false;
    r.convertedB0=converted;at(0x2b0bd8);std::uint32_t product=0;
    if(!g::detail::sourceParticleFogMultiply50LA(*r.halfRaw,*r.convertedB0,product))return false;
    r.product=product;r.multiplyClearMask=0x8000u;at(0x2b0bdc);r.fieldB8=product;
    //Actual full-string scan includes NUL; the reverse scan ignores slashes.
    const auto length=*f.pathLength;
    for(std::size_t i=0;i<=length;++i){at(0x2b0be0);at(0x2b0be4);at(0x2b0be8);
        const auto byte=f.path70[i];++r.forwardReadBytes;at(0x2b0bf4);at(0x2b0bf8);if(!byte)break;}
    at(0x2b0bfc);at(0x2b0c00);at(0x2b0c04);std::size_t suffix=length;
    for(;;){at(0x2b0c08);at(0x2b0c0c);if(!suffix)break;
        at(0x2b0c10);at(0x2b0c14);if(f.path70[suffix]=='.')break;at(0x2b0c18);--suffix;}
    at(0x2b0c20);at(0x2b0c24);at(0x2b0c28);
    r.suffixIndex=suffix;r.extensionIsNull=f.path70[suffix]!='.';r.extensionSelected=true;
    if(r.extensionIsNull)at(0x2b0c2c);else {at(0x2b0c30);at(0x2b0c34);}
    at(0x2b0c3c);at(0x2b0c40);at(0x2b0c44);at(0x308f00);r.comparisonEntered=true;
    if(r.extensionIsNull){at(0x308f08);return false;} //Actual NULL dereference boundary, not loader failure.
    using G=a::OriginalConfigProcessGlobals50LA;const G::WaterSprintfAccess key;
    constexpr char literal[]=".wtt";
    for(std::size_t i=0;;++i){
        const auto a=static_cast<unsigned char>(f.path70[suffix+i]);
        const auto b=static_cast<unsigned char>(literal[i]);std::int32_t foldedA=0,foldedB=0;
        if(a){
            at(0x308f34);if(!G::waterFold308F00(key,a,foldedA,pc)){at(pc);return false;}
            at(0x308f3c);if(!G::waterFold308F00(key,b,foldedB,pc)){at(pc);return false;}
            r.comparisonBytes=i+1u;if(foldedA==foldedB)continue;
        }else r.comparisonBytes=i+1u;
        //The mismatch/NUL exit re-reads and folds BOTH bytes through the
        //common308F7C..FB0 path, even after a prior nonzero loop comparison.
        r.comparisonReturnFoldEntered=true;
        at(0x308f90);if(!G::waterFold308F00(key,a,foldedA,pc)){at(pc);return false;}
        r.comparisonFinalA=foldedA;
        at(0x308f94);if(!G::waterFold308F00(key,b,foldedB,pc)){at(pc);return false;}
        r.comparisonFinalB=foldedB;
        at(0x308fac);at(0x308fb0);r.comparisonV0=foldedA-foldedB;break;
    }
    at(0x2b0c48);at(0x2b0c4c);
    if(*r.comparisonV0){
        at(0x2b0c84);at(0x2b0c88);at(0x2b0c8c);at(0x2b0c90);at(0x2b0c94);
        at(0x2ba8b0);r.alternateLoaderEntered=true;at(0x2ba8b4);r.alternateLoaderReturn=0;
        at(0x2b0c98);at(0x2b0c9c);at(0x2b0ca0);at(0x2b0ca4);at(0x2b0ca8);return false;
    }
    at(0x2b0c50);at(0x2b0c54);at(0x2b0c58);at(0x2b0c5c);at(0x2b0c60);
    at(0x2b92f0);r.wttLoaderEntered=true;at(0x2b9300);at(0x2b9304);at(0x2b9308);
    at(0x2b930c);at(0x2b9310);at(0x309230);
    //Conservative native span ends before498. Original strcpy has no length
    //check; refuse here without truncation or claiming an overflow branch.
    if(length>=r.filename398.size())return false;
    std::memcpy(r.filename398.data(),f.path70.data(),length+1u);
    r.filename398Known=length+1u;r.filenameCopyReturned=true;
    at(0x2b9314);return continueSourceWaterConfig2B9314(pc);
}

#include "original_source_world_water_config50la.inl"

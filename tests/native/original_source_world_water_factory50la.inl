//Included inside ra_cold_aggregate50LA with the actual private Owner and callers.
bool Owner::preflightSourceWaterFactoryNative()const noexcept {
    std::size_t providerCount=0;if(!sourceWaterProviderCountedNative(providerCount))return false;
    const auto& h=sourceWaterFactory274570_;const auto& call=sourcePostSkyTail_.water[0];
    if(!h.entered)return !h.arguments&&!h.numericLayout&&!h.environmentReloaded&&!h.directoryRead&&!h.directoryLength&&
        !h.formatted&&!h.pathLength&&!h.existence&&!h.lookupAttempted&&!h.existsV0&&
        !h.visibilityRead&&!h.visibilityWasNull&&!h.existing400Read&&!h.existing400&&
        !h.environment35CWritten&&!h.visibilityFallbackEntered&&!h.visibilityFallbackReturned&&
        !h.visibilityOwnerValidated&&!h.providerAllocationReached&&
        !h.diagnosticAttempted&&!h.diagnosticWritten;
    if(!call.dispatched||call.equalsNone!=false||!call.race8C||!call.argumentA2||
       !call.selectorLength||!sourcePostSkyTail_.argumentA3)return false;
    const auto expected=g::OriginalWaterReadbackLayout50LA::factoryArgumentsNumeric(
        *call.race8C,*sourcePostSkyTail_.argumentA3);
    if(h.arguments){
        if(!expected||h.arguments->constructorA2!=expected->constructorA2||
           h.arguments->constructorA3!=expected->constructorA3||
           h.arguments->initializerT0!=expected->initializerT0||
           h.arguments->initializerF12Bits!=expected->initializerF12Bits||!h.numericLayout||
           h.numericLayout->sourceArgumentA2Word()!=h.arguments->constructorA2||
           h.numericLayout->sourceArgumentA3Word()!=h.arguments->constructorA3)return false;
    }else if(h.numericLayout||h.environmentReloaded||h.directoryRead||h.formatted||h.existence)return false;
    if(h.directoryRead!=bool(h.directoryLength)||h.formatted!=bool(h.pathLength)||
       (h.directoryLength&&(*h.directoryLength>=worldText3C_.size()||!h.environmentReloaded)))return false;
    if(h.pathLength){
        if(!h.directoryRead||*h.pathLength>=h.path70.size()||
           *h.pathLength!=*h.directoryLength+*call.selectorLength||h.path70[*h.pathLength]!=0)return false;
        for(std::size_t i=0;i<*h.pathLength;++i)if(!h.path70[i])return false;
        if(std::memcmp(h.path70.data()+*h.directoryLength,call.selector.data(),*call.selectorLength+1))return false;
    }
    if((h.existence&&!h.formatted)||(h.lookupAttempted&&!h.existence))return false;
    if(h.existence){const auto& search=*h.existence;
        if(!search.preflightHistory()||(search.entered_&&!h.lookupAttempted))return false;
        if(search.entered_&&(!h.pathLength||search.name_.size()!=*h.pathLength||
           std::memcmp(search.name_.data(),h.path70.data(),*h.pathLength)))return false;
        if(h.existsV0&&(!h.lookupAttempted||search.returnV0_!=h.existsV0))return false;
    }else if(h.existsV0)return false;
    if((h.visibilityRead&&h.existsV0!=1u)||h.visibilityRead!=bool(h.visibilityWasNull)||
       (h.existing400Read&&h.existsV0!=0u)||h.existing400Read!=bool(h.existing400)||
       (h.diagnosticAttempted&&(!h.existing400||*h.existing400!=nullptr))||
       h.diagnosticWritten>sizeof("Water create failed in CreateWater.\n")-1u||(h.diagnosticWritten&&!h.diagnosticAttempted))return false;
    const auto& visibility=sourceVisibilityEnvironment_;
    if(h.environment35CWritten!=h.visibilityFallbackEntered||
       (h.environment35CWritten&&(h.existsV0!=1u||h.visibilityWasNull!=true))||
       (h.visibilityFallbackReturned&&(!h.visibilityFallbackEntered||
        visibility.caller!=SourceVisibilityEnvironmentCaller::water274714||!visibility.returned||
        !sourceVisibilityEnvironmentReturned_||sourceVisibilityEnvironmentReturnV0_!=1u))||
       (h.visibilityOwnerValidated&&(h.existsV0!=1u||!h.visibilityRead||!visibility.returned||
        !sourceVisibilityEnvironmentReturned_||sourceVisibilityEnvironmentReturnV0_!=1u||
        (*h.visibilityWasNull?!h.visibilityFallbackReturned:
            visibility.caller!=SourceVisibilityEnvironmentCaller::particle1B4E2C)))||
       h.providerAllocationReached!=h.visibilityOwnerValidated)return false;
    //The non-none factory returns only on the genuine missing-file path.
    //The found provider may be published, but its initializer has NOT returned.
    if(call.returnV0){
        if(h.existsV0!=0u||!h.existing400Read)return false;
        if(*h.existing400){if(*call.returnV0!=1u||h.diagnosticAttempted)return false;}
        else if(*call.returnV0!=0u||!h.diagnosticAttempted)return false;
    }
    //History survives native withdrawal. Never dereference a retained row or
    //require shared168 still equals the earlier result after another lookup.
    if(sourcePostParticleModelsPhase_==SourcePostParticleModelsPhase::nativeClosed)return true;
    if(h.environment35CWritten&&useEnvironment35C_!=1)return false;
    if(h.visibilityOwnerValidated&&(!worldApplication_||visibility.withdrawn||!visibility.tree||
       !visibility.node.owner||visibility.nativeNodeDetached||visibility.nativeAliasesWithdrawn||
       visibility.nativeConstructorReleased||worldApplication_->visibility443A98_!=visibility.tree.get()))return false;
    if(h.directoryLength){for(std::size_t n=0;n<=*h.directoryLength;++n){
        if(!worldText3C_[n])return false;
        if(h.formatted&&std::uint8_t(h.path70[n])!=*worldText3C_[n]&&n<*h.directoryLength)return false;
        if(n==*h.directoryLength&&*worldText3C_[n]!=0)return false;
    }}
    if(h.existence&&h.existence->entered_){
        if(!sourceTrackMountDomain_||!sourceTrackFile_||
           h.existence->resolverIdentity_!=sourceTrackMountDomain_->resolver3E1A70().get()||
           h.existence->trackIdentity_!=sourceTrackFile_.get())return false;
    }
    if(h.existing400Read&&(!worldWater400_||*worldWater400_!=*h.existing400))return false;
    return true;
}
bool Owner::continueSourceWaterFactory2745D4(std::uint32_t& pc)noexcept {
    auto& h=sourceWaterFactory274570_;auto& call=sourcePostSkyTail_.water[0];
    if(pc!=0x2745d4u||h.entered||call.equalsNone!=false||!call.race8C||
       !call.argumentA2||!sourcePostSkyTail_.argumentA3||!call.selectorLength||
       !preflightSourcePostParticleModelsNative())return false;
    const auto at=[&](std::uint32_t p)noexcept{pc=sourcePostParticleModelsPC_=p;};
    at(0x2745d4);h.entered=true;
    //Mode is A1=Race8C. Profile-derived A2 is retained for virtual+74 later,
    //NOT used to choose the dimension table. A3 is the real course selector.
    h.arguments=g::OriginalWaterReadbackLayout50LA::factoryArgumentsNumeric(
        *call.race8C,*sourcePostSkyTail_.argumentA3);
    if(!h.arguments)return false; //Unadmitted original out-of-table read.
    h.numericLayout.emplace(g::OriginalWaterReadbackLayout50LA::fromConstructorWords(
        h.arguments->constructorA2,h.arguments->constructorA3));
    at(0x2746c0); //Selected locals only. No provider constructor has executed.
    //Actual reload of the SAME source Environment at2746C0, not just the
    //historical selector copied before strcmp. No native lighting selector.
    if(!sourceEnvironmentPublished_||!nativeEnvironment3A4_||
       *nativeEnvironment3A4_!=sourceEnvironment3A4_.get()||!sourceEnvironment3A4_||
       sourceEnvironmentPhase_!=SourceEnvironmentPhase::returnedOne||!sourceWorldConfig_)return false;
    const char* selector=nullptr;std::size_t selectorLength=0;
    if(!sourceEnvironment3A4_->readSourceWaterFile110(worldEntity_,*sourceWorldConfig_,selector,selectorLength)||
       selectorLength!=*call.selectorLength||std::memcmp(selector,call.selector.data(),selectorLength+1))return false;
    h.environmentReloaded=true;
    at(0x2746c4);at(0x2746c8);at(0x2746cc);at(0x2746d0);at(0x2746d4);
    //Read only known World+3C bytes produced by the actual1B38DC path.
    std::size_t length=0;
    while(length<worldText3C_.size()){
        if(!worldText3C_[length])return false;
        if(*worldText3C_[length]==0)break;++length;
    }
    if(length==worldText3C_.size())return false;
    h.directoryLength=length;h.directoryRead=true;
    //3BA410 is literal%s%s, no inserted slash, normalization or truncation.
    //Original sprintf is unbounded.260 is a conservative native adaptation,
    //not a claim that a source bounded string function or overflow branch ran.
    if(*call.selectorLength>=h.path70.size()-length)return false;
    at(0x2746d8);at(0x308c90);
    for(std::size_t n=0;n<length;++n){const auto byte=*worldText3C_[n];std::memcpy(h.path70.data()+n,&byte,1);}
    std::memcpy(h.path70.data()+length,selector,selectorLength+1);
    h.pathLength=length+*call.selectorLength;h.formatted=true;
    at(0x2746dc);at(0x2746e0);
    if(!sourceTrackMountDomain_||sourceTrackMountLease_.closed_||!sourceTrackMountDomain_->prepared()||
       !sourceTrackFile_||!sourceLoadApplication_||
       sourceLoadApplication_->standardMaterialDomainOwner_!=sourceTrackMountDomain_||
       sourceTrackMountDomain_->archiveProvider3E1A70()!=sourceTrackMountLease_.provider_)return false;
    const auto resolver=sourceTrackMountDomain_->resolver3E1A70();if(!resolver)return false;
    std::string name;
    try{h.existence.reset(new a::OriginalSourceWaterExists131A40);
        name.assign(h.path70.data(),*h.pathLength);}catch(...){return false;}
    at(0x2746e4);at(0x2746e8);h.lookupAttempted=true;at(0x131a40);
    a::OriginalStandardTextureDiagnostic50LA d;
    if(!resolver->captureSourceWaterExists131A40(sourceTrackFile_,name,*h.existence,d)){
        at(d.sourcePC?d.sourcePC:0x131a40u);return false;}
    h.existsV0=h.existence->returnV0_;at(0x2746ec);at(0x2746f0);
    if(h.existsV0==1u){
        at(0x2746f4);at(0x2746f8);
        h.visibilityRead=true;h.visibilityWasNull=worldApplication_->visibility443A98_==nullptr;
        at(0x2746fc);at(0x274700);
        if(*h.visibilityWasNull){
            at(0x274704);at(0x274708);useEnvironment35C_=1;h.environment35CWritten=true;
            at(0x27470c);at(0x274710);
            //Same actual World vtable3CF110+100=272FC0. This is a fresh
            //water occurrence, never a replay of the earlier particle call.
            at(0x274714);h.visibilityFallbackEntered=true;at(0x274718);at(0x274714);
            if(!constructSourceVisibilityEnvironment272FC0(pc)){sourcePostParticleModelsPC_=pc;return false;}
            sourcePostParticleModelsPC_=pc;
            if(pc!=0x273174u||!sourceVisibilityEnvironmentReturned_||sourceVisibilityEnvironmentReturnV0_!=1u||
               sourceVisibilityEnvironment_.caller!=SourceVisibilityEnvironmentCaller::water274714||
               !preflightSourceVisibilityEnvironmentNative())return false;
            h.visibilityFallbackReturned=true;at(0x27471c);
        }
        at(0x274720);
        //A nonzero foreign singleton is not adopted as a typed source owner.
        //The successful fallback or the real earlier particle owner may be
        //validated here; this check itself grants no allocation or readiness.
        const auto& visibility=sourceVisibilityEnvironment_;
        if(!visibility.returned||visibility.withdrawn||!visibility.tree||!visibility.node.owner||
           visibility.nativeNodeDetached||visibility.nativeAliasesWithdrawn||visibility.nativeConstructorReleased||
           worldApplication_->visibility443A98_!=visibility.tree.get()||
           (!*h.visibilityWasNull&&visibility.caller!=SourceVisibilityEnvironmentCaller::particle1B4E2C)||
           !preflightSourceVisibilityEnvironmentNative())return false;
        h.visibilityOwnerValidated=true;h.providerAllocationReached=true;
        return continueSourceWaterProvider274720(pc);
    }
    if(h.existsV0!=0u)return false;
    at(0x274770);
    if(!worldReset270550Complete_||sourceLoadReset270550Phase_!=SourceLoadReset270550Phase::returned||
       !worldWater400_)return false;
    h.existing400Read=true;h.existing400=*worldWater400_;
    at(0x274774);at(0x274778);
    if(*h.existing400){call.returnV0=1;at(0x274794);return true;}
    at(0x27477c);at(0x274780);at(0x274784);at(0x1336c0);
    //Existing explicit native diagnostic policy: actual write attempt retained,
    //return ignored by the source caller. A short/error write is not success.
    static constexpr char message[]="Water create failed in CreateWater.\n";
    h.diagnosticAttempted=true;h.diagnosticWritten=std::fwrite(message,1,sizeof(message)-1,stdout);
    at(0x274788);at(0x27478c);call.returnV0=0;at(0x274794);return true;
}

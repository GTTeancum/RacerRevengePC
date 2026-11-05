//Included inside ra_cold_aggregate50LA by the Pool object leaf. This bounded
//native continuation consumes the SAME completed Pool and formatter940 role.
//String helpers below are explicit byte-effect projections, not guest/helper
//execution. The loader remains open before fresh global read2BB044.
bool Owner::preflightNativeWaterPoolNameHistory()const noexcept {
    using N=SourceWaterConfig::NativeWaterPoolName;using R=WaterPoolNameRefusal;
    using CS=N::CopySource;using CD=N::CopyDestination;
    const auto& n=sourceWaterConfig_.nativeWaterPoolName;
    const auto& f=sourceWaterConfig_.nativeWaterPoolPathFormat;
    const auto& c=sourceWaterConfig_.nativeWaterPoolEntity;
    const auto& s=sourceWaterConfig_.nativeWaterPoolScene;
    const auto virginCopy=[](const N::Copy& x)noexcept {
        return !x.callIssued&&!x.projected&&!x.returned&&!x.callPC&&!x.helperEntry&&!x.returnPC&&
            x.source==CS::none&&x.destination==CD::none&&!x.sourceOffset&&!x.destinationOffset&&
            !x.writeOffset&&!x.stored&&!x.resultOffset&&!x.frameBytes&&!x.nestedCopyCallPC&&
            !x.nestedCopyReturnPC&&!x.saveMask&&!x.restoreMask&&!x.stackReleased&&!x.nestedCopyReturned;
    };
    const auto copyMatches=[](const N::Copy& x,CS source,CD destination,std::uint32_t callPC,
        std::uint32_t helper,std::uint32_t returnPC,std::size_t destinationOffset,
        std::size_t writeOffset,std::size_t stored)noexcept {
        const bool append=helper==0x308fb8u;
        return x.callIssued&&x.projected&&x.returned&&x.source==source&&x.destination==destination&&
            x.callPC==callPC&&x.helperEntry==helper&&x.returnPC==returnPC&&!x.sourceOffset&&
            x.destinationOffset==destinationOffset&&x.writeOffset==writeOffset&&x.stored==stored&&
            x.resultOffset==destinationOffset&&x.frameBytes==(append?32u:0u)&&
            x.saveMask==(append?3u:0u)&&x.restoreMask==(append?3u:0u)&&x.stackReleased==append&&
            x.nestedCopyCallPC==(append?0x3090c8u:0u)&&x.nestedCopyReturnPC==(append?0x3090d0u:0u)&&
            x.nestedCopyReturned==append;
    };
    const auto noFinish=[&]()noexcept {return !n.atWord&&!n.v0Register;};
    const auto noDotOrSuffix=[&]()noexcept {
        return !n.dotCallIssued&&!n.dotReturned&&!n.dotArgument&&!n.dotReturnPC&&!n.lastDot&&
            !n.normalizedLength&&!n.normalized&&!n.extensionDestinationOffset&&!n.nameDestinationOffset&&
            virginCopy(n.extensionCopy)&&virginCopy(n.nameCopy)&&noFinish();
    };
    if(!n.attempted)return n.refusal==R::none&&!n.pc&&!n.wholeRoleIdentity&&!n.canonicalEntityIdentity&&
        !n.sourcePathIdentity&&!n.session&&!n.key&&!n.countedAtEntry&&!n.referenceAtEntry&&!n.enrolledAtEntry&&
        !n.wholeArgument&&!n.pathArgument&&!n.loaderCallIssued&&!n.argumentA2&&!n.argumentA3&&
        !n.callerPathOffset&&!n.loaderCallerReturnPC&&!n.loaderFrame&&!n.wholeInS0&&!n.pathInS1&&!n.flagInS4&&
        !n.loaderFrameBytes&&!n.loaderSaveMask&&!n.loaderRestoreMask&&!n.loaderStackReleased&&!n.loaderReturned&&
        !n.entityIdentityCallIssued&&!n.entityIdentityReturned&&!n.entityArgumentOffset&&!n.entityResultOffset&&
        !n.entityIdentityReturnPC&&!n.scratchOffset&&virginCopy(n.initialCopy)&&!n.initialLength&&!n.scratchKnown&&
        !n.lowerCalls&&!n.lowerReturns&&!n.lowerStores&&!n.lowerReadPC&&!n.lowerTerminatorRead&&
        !n.stoppedLowerInput&&noDotOrSuffix();
    //All these are historical identities/receipts. In particular, do not
    //dereference retired roles or consult the CURRENT process ctype table.
    if(!c.admitted||c.refusal!=WaterPoolEntityRefusal::none||c.pc!=0x289420u||
       !c.referenceAcquired||!c.countAcquired||!c.enrolled||!s.poolReturned||s.pc!=0x2b9d28u||
       !n.wholeRoleIdentity||n.wholeRoleIdentity!=s.wholeRoleIdentity||!n.canonicalEntityIdentity||
       n.sourcePathIdentity!=&f.output940||n.session!=c.session||n.key!=c.key||!n.session||!n.key||
       n.countedAtEntry!=1u||n.referenceAtEntry!=1u||!n.enrolledAtEntry||
       !n.wholeArgument||!n.pathArgument||!n.loaderCallIssued||n.argumentA2!=1u||n.argumentA3!=1u||
       n.callerPathOffset!=0x940u||n.loaderCallerReturnPC!=0x2b9d3cu||
       !n.loaderFrame||n.loaderFrameBytes!=496u||n.loaderSaveMask!=63u||n.loaderRestoreMask||
       n.loaderStackReleased||n.loaderReturned||!n.wholeInS0||!n.pathInS1||!n.flagInS4||
       !n.entityIdentityCallIssued||!n.entityIdentityReturned||n.entityArgumentOffset!=8u||
       n.entityResultOffset!=8u||n.entityIdentityReturnPC!=0x2bafc0u||n.scratchOffset!=0x60u||
       !f.attempted||f.pc!=0x2b9d0cu||!f.terminated||!f.wrapperReleased||
       f.phase!=SourceWaterConfig::NativeWaterPoolPathFormat::Phase::callerBoundary||
       f.copied>255u||f.known!=f.copied+1u||n.initialLength!=f.copied||
       n.lowerStores>f.copied||n.lowerReturns!=n.lowerStores||
       n.lowerReadPC!=(n.lowerCalls?0x30b168u:0u))return false;
    const auto length=f.copied;
    for(std::size_t i=0;i<length;++i)
        if(!f.output940[i]||(i<n.lowerStores&&f.output940[i]>127u))return false;
    if(f.output940[length]||!copyMatches(n.initialCopy,CS::formatter940,CD::scratch60,
        0x2bafc4u,0x309230u,0x2bafccu,0u,0u,length+1u))return false;
    const bool lowerStopped=n.refusal==R::nativeLowerNonAscii||n.refusal==R::nativeLowerTableUnknown;
    if(lowerStopped){
        if(n.pc!=0x30b168u||n.lowerStores>=length||n.lowerCalls!=n.lowerStores+1u||
           n.lowerTerminatorRead||n.stoppedLowerInput!=f.output940[n.lowerStores]||!noDotOrSuffix())return false;
        if((n.refusal==R::nativeLowerNonAscii)!=(f.output940[n.lowerStores]>127u))return false;
    }else if(n.lowerCalls!=length||n.lowerStores!=length||!n.lowerTerminatorRead||n.stoppedLowerInput)return false;
    std::optional<std::size_t> lastDot;
    if(!lowerStopped){
        //ASCII folding cannot change a dot. Scan the FULL immutable path,
        //including directory components; optional index zero is a match.
        for(std::size_t i=0;i<length;++i)if(f.output940[i]==0x2eu)lastDot=i;
        if(!n.dotCallIssued||!n.dotReturned||n.dotArgument!=0x2eu||n.dotReturnPC!=0x2bb000u||
           n.lastDot!=lastDot||n.normalizedLength!=(lastDot?*lastDot+4u:length+4u)||
           n.extensionDestinationOffset!=(lastDot?*lastDot:0u))return false;
    }
    const auto extensionBegin=lastDot?*lastDot:length;
    const auto normalizedLength=extensionBegin+4u;
    const auto expectedKnown=n.normalized?std::max(length+1u,normalizedLength+1u):length+1u;
    if(n.scratchKnown!=expectedKnown||n.scratchKnown>n.scratch60.size())return false;
    static constexpr std::array<std::uint8_t,5> slx{{0x2e,0x73,0x6c,0x78,0}};
    for(std::size_t i=0;i<n.scratchKnown;++i){
        std::uint8_t expected=0;
        if(i<f.known){
            expected=f.output940[i];
            if(i<n.lowerStores&&expected>='A'&&expected<='Z')expected=std::uint8_t(expected+32u);
        }else if(!n.normalized||i<extensionBegin||i>normalizedLength)return false;
        if(n.normalized&&i>=extensionBegin&&i<=normalizedLength)expected=slx[i-extensionBegin];
        if(n.scratch60[i]!=expected)return false;
    }
    if(lowerStopped)return true;
    const auto extensionCallPC=lastDot?0x2bb010u:0x2bb028u;
    if(n.refusal==R::nativeScratchAperture)return normalizedLength+1u>256u&&n.pc==extensionCallPC&&
        !n.normalized&&virginCopy(n.extensionCopy)&&!n.nameDestinationOffset&&virginCopy(n.nameCopy)&&noFinish();
    if(normalizedLength+1u>256u||!n.normalized||!copyMatches(n.extensionCopy,CS::slxLiteral3C0E80,
        CD::scratch60,extensionCallPC,lastDot?0x309230u:0x308fb8u,lastDot?0x2bb018u:0x2bb030u,
        lastDot?*lastDot:0u,extensionBegin,5u)||n.nameDestinationOffset!=0x38u)return false;
    if(n.refusal==R::nativeNameAperture)return normalizedLength+1u>0x88u&&n.pc==0x2bb034u&&
        virginCopy(n.nameCopy)&&noFinish();
    return n.refusal==R::none&&normalizedLength+1u<=0x88u&&
        copyMatches(n.nameCopy,CS::scratch60,CD::whole,0x2bb034u,0x309230u,0x2bb03cu,
                    0x38u,0x38u,normalizedLength+1u)&&n.pc==0x2bb044u&&
        n.atWord==0x003e0000u&&n.v0Register==std::numeric_limits<std::uint64_t>::max();
}

Owner::WaterPoolNameResult Owner::constructWaterPoolNameNativeOnly(WaterArrayBackingMode mode)noexcept {
    using N=SourceWaterConfig::NativeWaterPoolName;using R=WaterPoolNameRefusal;
    using CS=N::CopySource;using CD=N::CopyDestination;using G=a::OriginalConfigProcessGlobals50LA;
    WaterPoolNameResult result;auto& n=sourceWaterConfig_.nativeWaterPoolName;
    const auto& f=sourceWaterConfig_.nativeWaterPoolPathFormat;
    const auto& c=sourceWaterConfig_.nativeWaterPoolEntity;
    const auto& s=sourceWaterConfig_.nativeWaterPoolScene;
    const auto& b=sourceWaterConfig_.nativeWaterPoolBacking;
    const auto finish=[&](WaterPoolNameResult::Status status)noexcept {
        result.status=status;result.refusal=n.refusal;result.originalSourceStop=0x30492cu;
        result.allocatorStop=sourceWaterConfig_.nativeWaterPoolAllocationEntry.pc;
        result.formatterStop=f.pc;result.entityPrefixStop=c.pc;result.sceneStop=s.pc;result.adaptedPC=n.pc;
        result.scratchKnown=n.scratchKnown;result.lowerCalls=n.lowerCalls;result.lowerStores=n.lowerStores;
        result.nameBytesWritten=n.nameCopy.stored;result.initialLength=n.initialLength;
        result.normalizedLength=n.normalizedLength;result.lastDot=n.lastDot;result.normalized=n.normalized;
        result.nameCopied=n.nameCopy.returned;result.nativeRetired=b.nativeRetired;return result;
    };
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||nativeScbThread_!=std::this_thread::get_id()||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_)return result;
    std::size_t counted=0;if(!sourceWaterPoolCountedNative(counted))return result;
    //One shot, including refusals. Historical results survive native Pool
    //retirement and do not need the old table provenance to remain current.
    if(n.attempted)return finish(WaterPoolNameResult::Status::alreadyAttempted);
    if(!waterPoolInvocationCurrentNative()||!preflightSourceWaterConfigNative()||
       !preflightSourceWorldParticleCountNative()||counted!=1u||!c.admitted||c.pc!=0x289420u||
       !s.poolReturned||s.pc!=0x2b9d28u||!b.nativeAllocated||b.nativeRetired||!sourceWaterProvider_->nativePool||
       !f.terminated||f.pc!=0x2b9d0cu||f.copied>255u||f.known!=f.copied+1u)return result;
    for(std::size_t i=0;i<f.copied;++i)if(!f.output940[i])return result;
    if(f.output940[f.copied])return result;
    auto& p=*sourceWaterProvider_->nativePool;
    if(!p.wholeArgument||p.wholeArgument->role!=&p.role||p.wholeArgument->offset||
       !p.entityArgument||p.entityArgument->role!=&p.role||p.entityArgument->offset!=8u)return result;
    const G::WaterSprintfAccess access;
    const auto refuse=[&](R reason)noexcept {
        n.refusal=reason;return finish(WaterPoolNameResult::Status::nativeAdmissionStopped);
    };
    const auto beginCopy=[](N::Copy& x,CS source,CD destination,std::uint32_t callPC,
        std::uint32_t helper,std::size_t destinationOffset,std::size_t writeOffset)noexcept {
        x.callIssued=true;x.callPC=callPC;x.helperEntry=helper;x.source=source;x.destination=destination;
        x.destinationOffset=destinationOffset;x.writeOffset=writeOffset;
    };
    const auto finishCopy=[](N::Copy& x,std::uint32_t returnPC)noexcept {
        x.projected=true;x.returned=true;x.returnPC=returnPC;x.resultOffset=x.destinationOffset;
    };
    n.attempted=true;n.wholeRoleIdentity=&p.role;n.canonicalEntityIdentity=&p.entity;
    n.sourcePathIdentity=&f.output940;n.session=c.session;n.key=c.key;
    n.countedAtEntry=counted;n.referenceAtEntry=p.entity.reference00;n.enrolledAtEntry=c.enrolled;
    n.pc=0x2b9d28;n.argumentA2=1u;
    n.pc=0x2b9d2c;n.callerPathOffset=0x940u;n.pathArgument=true;
    n.pc=0x2b9d30;n.wholeArgument=true;
    n.pc=0x2b9d34;n.loaderCallIssued=true;n.loaderCallerReturnPC=0x2b9d3cu;
    n.pc=0x2b9d38;n.argumentA3=n.argumentA2;
    n.pc=0x2baf90;n.loaderFrame=true;n.loaderFrameBytes=496u;
    //Six SQ saves: ra+50, s4+40, s3+30, s2+20, s1+10, s0+00.
    //Saved register values remain opaque; no guest stack address is invented.
    n.pc=0x2baf94;n.loaderSaveMask|=1u;n.pc=0x2baf98;n.loaderSaveMask|=2u;
    n.pc=0x2baf9c;n.loaderSaveMask|=4u;n.pc=0x2bafa0;n.loaderSaveMask|=8u;
    n.pc=0x2bafa4;n.loaderSaveMask|=16u;n.pc=0x2bafa8;n.loaderSaveMask|=32u;
    n.pc=0x2bafac;n.wholeInS0=true;
    n.pc=0x2bafb0;n.entityArgumentOffset=8u;
    n.pc=0x2bafb4;n.pathInS1=true;
    n.pc=0x2bafb8;n.entityIdentityCallIssued=true;
    n.pc=0x2bafbc;n.flagInS4=true;
    //141C20 JR /141C24 POR returns the existing Entity+8. It performs
    //no read/store/retain, and does not replace the whole role held in s0.
    n.entityResultOffset=8u;n.entityIdentityReturned=true;n.entityIdentityReturnPC=0x2bafc0u;
    n.pc=0x2bafc0;n.scratchOffset=0x60u;
    n.pc=0x2bafc4;beginCopy(n.initialCopy,CS::formatter940,CD::scratch60,n.pc,0x309230u,0u,0u);
    n.pc=0x2bafc8;n.initialLength=f.copied;
    //The all-alignment proof in the loader trace quantifies over arbitrary
    //post-NUL tails: optimized blocks containing NUL are NEVER stored. Read
    //only the known L+1 prefix, not the rounded lookahead or host unknowns.
    //This does not change the older conservative shader-copy contract.
    for(std::size_t i=0;i<f.known;++i){n.scratch60[i]=f.output940[i];++n.initialCopy.stored;}
    n.scratchKnown=f.known;finishCopy(n.initialCopy,0x2bafccu);
    n.pc=0x2bafcc;n.pc=0x2bafd0; //s1 now denotes loader-local scratch+60.
    for(std::size_t i=0;;++i){
        n.pc=0x2bafe8;const auto byte=n.scratch60[i];
        n.pc=0x2bafec;n.pc=0x2baff0;
        if(!byte){n.lowerTerminatorRead=true;break;}
        n.pc=0x2bafd4;++n.lowerCalls;n.pc=0x2bafd8;
        //Source LB is signed. The existing key admits only ASCII, so a
        //negative-index byte is refused BEFORE the unsupported table read.
        //The API name308F00 is not this call's PC; its actual read is30B168.
        n.pc=0x30b168;n.lowerReadPC=n.pc;std::int32_t folded=0;
        if(!G::waterFold308F00(access,byte,folded,n.pc)){
            n.stoppedLowerInput=byte;
            return refuse(byte>127u?R::nativeLowerNonAscii:R::nativeLowerTableUnknown);
        }
        n.pc=0x30b170;n.pc=0x30b174;++n.lowerReturns;
        n.pc=0x2bafdc;n.scratch60[i]=static_cast<std::uint8_t>(folded);++n.lowerStores;
        n.pc=0x2bafe0;n.pc=0x2bafe4;
    }
    n.pc=0x2baff4;n.pc=0x2baff8;n.dotCallIssued=true;
    n.pc=0x2baffc;n.dotArgument=0x2eu;
    //1169D0 is a last-occurrence search over the entire path, including
    //directory dots. This projection needs only known bytes through NUL.
    for(std::size_t i=0;i<*n.initialLength;++i)if(n.scratch60[i]==0x2eu)n.lastDot=i;
    n.dotReturned=true;n.dotReturnPC=0x2bb000u;n.pc=0x2bb000;n.pc=0x2bb004;
    const auto extensionBegin=n.lastDot?*n.lastDot:*n.initialLength;
    n.normalizedLength=extensionBegin+4u;
    if(n.lastDot){
        n.pc=0x2bb008;n.extensionDestinationOffset=*n.lastDot;n.pc=0x2bb00c;n.pc=0x2bb010;
    }else{
        n.pc=0x2bb020;n.pc=0x2bb024;n.extensionDestinationOffset=0u;n.pc=0x2bb028;
    }
    //Explicit NATIVE scratch aperture, before JAL and its delay argument.
    //No original bounds branch, truncation, helper return or NULL is minted.
    if(*n.normalizedLength+1u>n.scratch60.size())return refuse(R::nativeScratchAperture);
    beginCopy(n.extensionCopy,CS::slxLiteral3C0E80,CD::scratch60,n.pc,
              n.lastDot?0x309230u:0x308fb8u,*n.extensionDestinationOffset,extensionBegin);
    n.pc=n.lastDot?0x2bb014u:0x2bb02cu;
    if(!n.lastDot){
        //308FB8's scanner writes nothing. It finds the exact known NUL
        //regardless of arbitrary lookahead tails, then JAL3090C8 calls309230.
        n.extensionCopy.frameBytes=32u;n.extensionCopy.saveMask=3u;
        n.extensionCopy.nestedCopyCallPC=0x3090c8u;
    }
    static constexpr std::array<std::uint8_t,5> slx{{0x2e,0x73,0x6c,0x78,0}};
    for(std::size_t i=0;i<slx.size();++i){n.scratch60[extensionBegin+i]=slx[i];++n.extensionCopy.stored;}
    //A shorter replacement retains the copied/lowered suffix after its new
    //NUL. No clearing and no loss of already-known bytes is permitted.
    n.scratchKnown=std::max(n.scratchKnown,*n.normalizedLength+1u);n.normalized=true;
    if(!n.lastDot){
        n.extensionCopy.nestedCopyReturned=true;n.extensionCopy.nestedCopyReturnPC=0x3090d0u;
        n.extensionCopy.restoreMask=3u;n.extensionCopy.stackReleased=true; //JR3090DC, delay3090E0.
    }
    finishCopy(n.extensionCopy,n.lastDot?0x2bb018u:0x2bb030u);
    if(n.lastDot){n.pc=0x2bb018;n.pc=0x2bb01c;}else n.pc=0x2bb030;
    n.nameDestinationOffset=0x38u;n.pc=0x2bb034;
    //Source has NO size argument and NO proved128-byte field. Admit only
    //the136-byte nonoverlap aperture whole+[38,C0), before the first matrix.
    //Refusal is before JAL2BB034 and before delay2BB038 passes scratch a1.
    if(*n.normalizedLength+1u>0xc0u-0x38u)return refuse(R::nativeNameAperture);
    beginCopy(n.nameCopy,CS::scratch60,CD::whole,n.pc,0x309230u,0x38u,0x38u);
    n.pc=0x2bb038;auto* bytes=static_cast<unsigned char*>(p.payload.get());
    for(std::size_t i=0;i<=*n.normalizedLength;++i){
        const auto value=n.scratch60[i];bytes[0x38u+i]=value;
        p.knownMask[0x38u+i]=0xffu;p.knownBits[0x38u+i]=value;++n.nameCopy.stored;
    }
    finishCopy(n.nameCopy,0x2bb03cu);
    n.pc=0x2bb03c;n.atWord=0x003e0000u;
    n.pc=0x2bb040;n.v0Register=std::numeric_limits<std::uint64_t>::max();
    n.pc=0x2bb044; //BEFORE fresh global3E1A70 read. No1E0 sentinel store,
    //resource access/alias, loader return, slot publication or transform.
    return finish(WaterPoolNameResult::Status::nativeResourceBoundary);
}

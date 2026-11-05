//Included after the initializer leaf in the actual World Owner namespace.
bool Owner::preflightSourceWaterConfigNative()const noexcept {
    const auto& h=sourceWaterConfig_;const auto& r=sourceWaterProviderInitializer_;
    if(!h.constructAttempted)return !sourceWaterConfigOwner_&&!h.constructReturned&&!h.nativeBackingRefused&&
        !h.openAttempted&&!h.openReturn&&!h.loaderReturn&&!h.loaderRestoreMask&&!h.loaderStackReleased&&!h.nativeWithdrawn;
    if(!r.filenameCopyReturned||!r.constructorThis||h.constructReturned==h.nativeBackingRefused||
       bool(sourceWaterConfigOwner_)!=h.constructReturned||(h.openAttempted&&!h.constructReturned)||
       (h.openReturn&&!h.openAttempted)||(h.loaderReturn&&(!h.openReturn||*h.openReturn||*h.loaderReturn))||
       h.loaderRestoreMask!=(h.loaderReturn?15u:0u)||h.loaderStackReleased!=bool(h.loaderReturn))return false;
    if(h.nativeBackingRefused)return r.pc==0x2b9314u;
    const auto& config=*sourceWaterConfigOwner_;
    if(!config.preflightNative(r.constructorThis)||h.nativeWithdrawn!=config.nativeClosed_||
       h.openReturn!=config.openReturned_||bool(h.loaderReturn)!=config.destructorReturned_)return false;
    if(h.loaderReturn)return r.pc==0x2b0c74u;
    if(!config.attempted_)return r.pc==0x2b9324u;
    return r.pc==config.pc_;
}
bool Owner::withdrawSourceWaterConfigNative()noexcept {
    if(!preflightSourceWaterConfigNative())return false;
    auto& h=sourceWaterConfig_;if(!sourceWaterConfigOwner_||h.nativeWithdrawn)return true;
    a::OriginalStandardTextureDiagnostic50LA d;
    if(!sourceWaterConfigOwner_->abortNative(d))return false;
    h.nativeWithdrawn=true;return true;
}
bool Owner::continueSourceWaterConfig2B9314(std::uint32_t& pc)noexcept {
    auto& h=sourceWaterConfig_;auto& r=sourceWaterProviderInitializer_;
    const auto& factory=sourceWaterFactory274570_;
    if(pc!=0x2b9314u||r.pc!=pc||sourcePostParticleModelsPC_!=pc||h.constructAttempted||
       !r.filenameCopyReturned||!sourceWaterProvider_||!sourceTrackFile_||!sourceTrackMountDomain_||
       !sourceLoadApplication_||sourceLoadApplication_->standardMaterialDomainOwner_!=sourceTrackMountDomain_||
       sourceTrackMountLease_.closed_||!sourceTrackMountDomain_->prepared()||
       sourceTrackMountDomain_->archiveProvider3E1A70()!=sourceTrackMountLease_.provider_||
       !preflightSourcePostParticleModelsNative()||!preflightSourceWorldParticleCountNative())return false;
    const auto at=[&](std::uint32_t value)noexcept {
        pc=sourcePostParticleModelsPC_=sourceWaterProvider274720_.pc=sourceWaterProviderNumeric_.pc=r.pc=value;
    };
    h.constructAttempted=true;
    //Native storage for the actual stack Config, not source new/malloc.
    sourceWaterConfigOwner_.reset(new(std::nothrow) a::OriginalWaterSourceConfig50LA(r.constructorThis));
    if(!sourceWaterConfigOwner_){h.nativeBackingRefused=true;return false;}
    h.constructReturned=true;at(0x2b931c);at(0x2b9320);at(0x2b9324);h.openAttempted=true;
    std::string filename;try{filename.assign(factory.path70.data(),*factory.pathLength);}
    catch(...){return false;} //No source1577D0 return/NULL is fabricated.
    auto& config=*sourceWaterConfigOwner_;a::OriginalStandardTextureDiagnostic50LA d;
    if(!config.begin1577D0(sourceTrackMountDomain_->resolver3E1A70(),sourceTrackMountLease_.provider_,
                         sourceTrackFile_,filename,d)){
        at(config.attempted_?config.pc_:(d.sourcePC?d.sourcePC:0x2b9324u));return false;
    }
    h.openReturn=config.openReturned_;at(0x2b932c);at(0x2b9330);
    //Only genuine source false returns are implemented here. Text parsing
    //and nonzero binary allocation are bounded inside their exact occurrence.
    if(!h.openReturn||*h.openReturn)return false;
    at(0x2b9334);at(0x2b9338);at(0x2b933c);at(0x2b9340);
    if(!config.closeRejectedStack1573B0(d)){at(config.pc_);return false;}
    at(0x2b9344);at(0x2b9348);h.loaderReturn=0;
    at(0x2b9f14);h.loaderRestoreMask|=1;at(0x2b9f18);h.loaderRestoreMask|=2;
    at(0x2b9f1c);h.loaderRestoreMask|=4;at(0x2b9f20);h.loaderRestoreMask|=8;
    at(0x2b9f24);at(0x2b9f28);h.loaderStackReleased=true;
    at(0x2b0c64);at(0x2b0c68);at(0x2b0c6c);at(0x2b0c70);at(0x2b0c74);
    return false; //Before1336C0 diagnostic. No base/initializer/factory return.
}

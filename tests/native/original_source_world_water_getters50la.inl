//Three158B80 occurrences in the real2B9300 Water loader, SAME stack Config.
bool Owner::preflightSourceWaterGettersNative()const noexcept {
    if(!preflightSourceWaterOpacityNative())return false;
    const auto& h=sourceWaterConfig_;const auto& r=sourceWaterProviderInitializer_;
    if(!h.gettersEntered){
        if(h.heightRead2B9380||h.loadPublications[0]||h.loadPublications[1]||h.loadPublications[2]||
           (sourceWaterProvider_&&sourceWaterProvider_->baseHeight3C))return false;
        if(sourceWaterConfigOwner_)for(const auto& f:sourceWaterConfigOwner_->floatGetters_)if(f.attempted)return false;
        return true;
    }
    if(!sourceWaterConfigOwner_||!h.constructReturned||h.openReturn!=true||h.loaderReturn)return false;
    const auto& config=*sourceWaterConfigOwner_;const auto& f=config.floatGetters_;
    if(!config.sectionReturn_||!f[0].attempted)return false;
    if(!sourceWaterProvider274720_.nativeWithdrawn){
        if(!sourceWaterProvider_||!worldApplication_||
           sourceWaterProvider_->baseHeight3C!=f[0].result)return false;
    }
    if(h.heightRead2B9380&&h.heightRead2B9380!=f[0].result)return false;
    for(std::size_t i=0;i<3u;++i){
        const auto& receipt=h.loadPublications[i];
        if(receipt){
            if(!*receipt||!h.heightRead2B9380||(i&&!f[i].returned))return false;
            if(!sourceWaterProvider274720_.nativeWithdrawn){
                const auto& p=worldApplication_->sourceWaterProcessScalars_;
                const auto expected=i?f[i].result:h.heightRead2B9380;
                if(p.publications[i+2u]<*receipt||
                   (p.publications[i+2u]==*receipt&&!p.unknown&&p.words[i+2u]!=expected))return false;
            }
        }
    }
    //A native stop retains every earlier store. Withdrawal only releases
    //native custody; history and App process publications are not rolled back.
    if(!f[0].returned)return !h.heightRead2B9380&&!h.loadPublications[0]&&!f[1].attempted&&r.pc==config.pc_;
    if(!h.heightRead2B9380)return false;
    if(!h.loadPublications[0])return !f[1].attempted&&!h.loadPublications[1]&&!f[2].attempted&&r.pc==0x2b93a0u;
    if(!f[1].attempted)return !h.loadPublications[1]&&!f[2].attempted&&r.pc==0x158b80u;
    if(!f[1].returned)return !h.loadPublications[1]&&!f[2].attempted&&r.pc==config.pc_;
    if(!h.loadPublications[1])return false;
    if(!f[2].attempted)return !h.loadPublications[2]&&r.pc==0x158b80u;
    if(!f[2].returned)return !h.loadPublications[2]&&r.pc==config.pc_;
    return h.loadPublications[2]&&(h.opacityEntered||r.pc==0x2b93d8u);
}
bool Owner::continueSourceWaterGetters2B9370(std::uint32_t& pc)noexcept {
    auto& h=sourceWaterConfig_;auto& r=sourceWaterProviderInitializer_;
    if(pc!=0x2b9370u||r.pc!=pc||sourcePostParticleModelsPC_!=pc||h.gettersEntered||
       !sourceWaterConfigOwner_||!sourceWaterProvider_||!worldApplication_||!sourceLoadApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||h.openReturn!=true||h.nativeWithdrawn||
       !sourceWaterConfigOwner_->sectionReturn_||nativeScbThread_!=std::this_thread::get_id()||
       !preflightSourceWaterConfigNative())return false;
    const auto at=[&](std::uint32_t value)noexcept {
        pc=sourcePostParticleModelsPC_=sourceWaterProvider274720_.pc=sourceWaterProviderNumeric_.pc=r.pc=value;
    };
    auto& config=*sourceWaterConfigOwner_;auto& process=worldApplication_->sourceWaterProcessScalars_;
    a::OriginalStandardTextureDiagnostic50LA d;h.gettersEntered=true;
    if(!config.readWaterFloat158B80(0,sourceWaterProvider_->baseHeight3C,d)){at(config.pc_);return false;}
    at(0x2b9378);at(0x2b937c);at(0x2b9380);h.heightRead2B9380=sourceWaterProvider_->baseHeight3C;
    at(0x2b9384);at(0x2b9388);at(0x2b938c);at(0x2b9390);at(0x2b9394);at(0x2b9398);at(0x2b939c);
    //Second JAL's DELAY SLOT publishes the actual just-loaded Water+3C.
    //It precedes all second-getter reads and survives a later refusal there.
    at(0x2b93a0);if(!worldApplication_->waterLoadScalarAdmittedNative(2))return false;
    process.words[2]=h.heightRead2B9380;h.loadPublications[0]=++process.publications[2];
    at(0x158b80);if(!worldApplication_->waterLoadScalarAdmittedNative(3))return false;
    if(!config.readWaterFloat158B80(1,process.words[3],d)){at(config.pc_);return false;}
    h.loadPublications[1]=++process.publications[3]; //Native receipt; no fallible gap after source store.
    at(0x2b93a4);at(0x2b93a8);at(0x2b93ac);at(0x2b93b0);at(0x2b93b4);at(0x2b93b8);at(0x2b93bc);at(0x2b93c0);
    at(0x158b80);if(!worldApplication_->waterLoadScalarAdmittedNative(4))return false;
    if(!config.readWaterFloat158B80(2,process.words[4],d)){at(config.pc_);return false;}
    h.loadPublications[2]=++process.publications[4];
    at(0x2b93c4);at(0x2b93c8);at(0x2b93cc);at(0x2b93d0);at(0x2b93d4);at(0x2b93d8);
    return continueSourceWaterOpacity2B93D8(pc);
}

#include "original_source_world_water_opacity50la.inl"

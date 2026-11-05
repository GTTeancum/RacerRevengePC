//Actual fourth/fifth158B80 occurrences, then retained source SUB.S operands.
bool Owner::preflightSourceWaterOpacityNative()const noexcept {
    if(!preflightSourceWaterScalarTailNative())return false;
    const auto& h=sourceWaterConfig_;const auto& r=sourceWaterProviderInitializer_;
    if(!h.opacityEntered){
        if(h.maxOpacityA40||h.opacityRead2B93FC||h.maxRead2B9408||h.opacityReload2B9418||h.opacityPublication||
           (sourceWaterProvider_&&sourceWaterProvider_->baseOpacity60))return false;
        if(sourceWaterConfigOwner_){const auto& f=sourceWaterConfigOwner_->floatGetters_;
            if(f[3].attempted||f[4].attempted)return false;}
        return true;
    }
    if(!h.gettersEntered||!h.loadPublications[2]||!sourceWaterConfigOwner_)return false;
    const auto& config=*sourceWaterConfigOwner_;const auto& base=config.floatGetters_[3];
    const auto& maximum=config.floatGetters_[4];
    if(!base.attempted||h.maxOpacityA40!=maximum.result)return false;
    if(!sourceWaterProvider274720_.nativeWithdrawn){
        if(!sourceWaterProvider_||!worldApplication_||sourceWaterProvider_->baseOpacity60!=base.result)return false;
    }
    if((h.opacityRead2B93FC&&h.opacityRead2B93FC!=base.result)||
       (h.maxRead2B9408&&h.maxRead2B9408!=h.maxOpacityA40)||
       (h.opacityReload2B9418&&h.opacityReload2B9418!=base.result))return false;
    if(h.opacityPublication){
        if(!*h.opacityPublication||!h.opacityRead2B93FC||!h.maxRead2B9408||!h.opacityReload2B9418)return false;
        if(!sourceWaterProvider274720_.nativeWithdrawn){
            const auto& process=worldApplication_->sourceWaterProcessScalars_;
            if(process.publications[5]<*h.opacityPublication||
               (process.publications[5]==*h.opacityPublication&&!process.unknown&&
                process.words[5]!=h.opacityRead2B93FC))return false;
        }
    }
    if(!base.returned)return !maximum.attempted&&!h.opacityRead2B93FC&&!h.maxRead2B9408&&
        !h.opacityReload2B9418&&!h.opacityPublication&&r.pc==config.pc_;
    if(!maximum.attempted)return false; //No fallible operation between fourth return and fifth call.
    if(!maximum.returned)return !h.opacityRead2B93FC&&!h.maxRead2B9408&&
        !h.opacityReload2B9418&&!h.opacityPublication&&r.pc==config.pc_;
    if(!h.opacityRead2B93FC||!h.maxRead2B9408)return false;
    if(!h.opacityPublication)return !h.opacityReload2B9418&&r.pc==0x2b9414u;
    return h.scalarTailEntered||r.pc==0x2b9428u; //Next leaf validates any reached subtraction.
}
bool Owner::continueSourceWaterOpacity2B93D8(std::uint32_t& pc)noexcept {
    auto& h=sourceWaterConfig_;auto& r=sourceWaterProviderInitializer_;
    if(pc!=0x2b93d8u||r.pc!=pc||sourcePostParticleModelsPC_!=pc||h.opacityEntered||h.nativeWithdrawn||
       !sourceWaterConfigOwner_||!sourceWaterProvider_||!worldApplication_||!sourceLoadApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||nativeScbThread_!=std::this_thread::get_id()||
       !preflightSourceWaterConfigNative())return false;
    const auto at=[&](std::uint32_t value)noexcept {
        pc=sourcePostParticleModelsPC_=sourceWaterProvider274720_.pc=sourceWaterProviderNumeric_.pc=r.pc=value;
    };
    auto& config=*sourceWaterConfigOwner_;a::OriginalStandardTextureDiagnostic50LA d;
    h.opacityEntered=true;at(0x2b93dc); //Actual a2=Water+60 in fourth JAL's delay slot.
    if(!config.readWaterFloat158B80(3,sourceWaterProvider_->baseOpacity60,d)){at(config.pc_);return false;}
    at(0x2b93e0);at(0x2b93e4);at(0x2b93e8);at(0x2b93ec);at(0x2b93f0);at(0x2b93f4);at(0x2b93f8);
    //Actual sp+A40 destination, not a detached Config result later adopted.
    if(!config.readWaterFloat158B80(4,h.maxOpacityA40,d)){at(config.pc_);return false;}
    at(0x2b93fc);h.opacityRead2B93FC=sourceWaterProvider_->baseOpacity60;
    at(0x2b9400);at(0x2b9404);at(0x2b9408);h.maxRead2B9408=h.maxOpacityA40;
    at(0x2b940c);at(0x2b9410);at(0x2b9414);
    if(!worldApplication_->waterLoadScalarAdmittedNative(5))return false;
    auto& process=worldApplication_->sourceWaterProcessScalars_;
    process.words[5]=h.opacityRead2B93FC;h.opacityPublication=++process.publications[5];
    at(0x2b9418);h.opacityReload2B9418=sourceWaterProvider_->baseOpacity60;
    at(0x2b941c);at(0x2b9420);at(0x2b9424);at(0x2b9428);
    return continueSourceWaterScalarTail2B9428(pc);
}

#include "original_source_world_water_scalar_tail50la.inl"

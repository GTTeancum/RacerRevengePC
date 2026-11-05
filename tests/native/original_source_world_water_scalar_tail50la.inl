//Original SUB.S, its next-call delay store, and ten further float getters.
bool Owner::preflightSourceWaterScalarTailNative()const noexcept {
    if(!preflightSourceWaterColorsNative())return false;
    const auto& h=sourceWaterConfig_;const auto& r=sourceWaterProviderInitializer_;
    //Constructor channel zeros and subsequent getters address the SAME lanes.
    //Their zeroWords entries remain constructor history, not current values.
    if(sourceWaterProvider_){
        for(std::size_t i=0;i<4u;++i){
            std::optional<std::uint32_t> expected;
            if(sourceWaterConfigOwner_)expected=sourceWaterConfigOwner_->floatGetters_[5u+i].result;
            if(!expected&&sourceWaterProvider274720_.zeroWordsWritten>17u+i)expected=0u;
            if(sourceWaterProvider_->channelScale98[i]!=expected)return false;
        }
        for(std::size_t i=0;i<5u;++i){
            const auto expected=sourceWaterConfigOwner_?sourceWaterConfigOwner_->floatGetters_[9u+i].result:
                std::optional<std::uint32_t>{};
            if(sourceWaterProvider_->framesPerSecond84[i]!=expected)return false;
        }
    }
    if(!h.scalarTailEntered){
        if(h.opacitySubtractAttempted||h.opacityDifference||h.opacitySubtractClearMask||
           h.opacityRangePublication||h.wakeScalePublication)return false;
        if(sourceWaterConfigOwner_)for(std::size_t i=5;i<15u;++i)
            if(sourceWaterConfigOwner_->floatGetters_[i].attempted)return false;
        return true;
    }
    if(!h.opacityEntered||!h.opacityPublication||!h.maxRead2B9408||!h.opacityReload2B9418||
       !sourceWaterConfigOwner_||!h.opacitySubtractAttempted)return false;
    const auto& config=*sourceWaterConfigOwner_;const auto& f=config.floatGetters_;
    if(!sourceWaterProvider274720_.nativeWithdrawn&&(!sourceWaterProvider_||!worldApplication_))return false;
    if(bool(h.opacityDifference)!=bool(h.opacitySubtractClearMask))return false;
    if(!h.opacityDifference)return !h.opacityRangePublication&&!h.wakeScalePublication&&!f[5].attempted&&r.pc==0x2b9428u;
    std::uint32_t result=0,mask=0;
    if(!g::detail::sourceWaterOpacitySubtract50LA(*h.maxRead2B9408,*h.opacityReload2B9418,result,mask)||
       h.opacityDifference!=result||h.opacitySubtractClearMask!=mask)return false;
    for(std::size_t i=0;i<2u;++i){
        const auto& receipt=i?h.wakeScalePublication:h.opacityRangePublication;
        if(!receipt)continue;
        if(!*receipt||(i&&!f[14].returned))return false;
        if(!sourceWaterProvider274720_.nativeWithdrawn){
            const auto& process=worldApplication_->sourceWaterProcessScalars_;
            const auto expected=i?f[14].result:h.opacityDifference;
            if(process.publications[6u+i]<*receipt||
               (process.publications[6u+i]==*receipt&&!process.unknown&&process.words[6u+i]!=expected))return false;
        }
    }
    if(!h.opacityRangePublication)return !h.wakeScalePublication&&!f[5].attempted&&r.pc==0x2b9430u;
    for(std::size_t i=5;i<14u;++i){
        if(!f[i].attempted)return false; //No fallible operation between these caller returns/calls.
        if(!f[i].returned)return !h.wakeScalePublication&&r.pc==config.pc_;
    }
    if(!f[14].attempted)return !h.wakeScalePublication&&r.pc==0x158b80u;
    if(!f[14].returned)return !h.wakeScalePublication&&r.pc==config.pc_;
    return h.wakeScalePublication&&(h.colorGettersEntered||r.pc==0x2b9528u);
}
bool Owner::continueSourceWaterScalarTail2B9428(std::uint32_t& pc)noexcept {
    auto& h=sourceWaterConfig_;auto& r=sourceWaterProviderInitializer_;
    if(pc!=0x2b9428u||r.pc!=pc||sourcePostParticleModelsPC_!=pc||h.scalarTailEntered||h.nativeWithdrawn||
       !sourceWaterConfigOwner_||!sourceWaterProvider_||!worldApplication_||!sourceLoadApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||nativeScbThread_!=std::this_thread::get_id()||
       !preflightSourceWaterConfigNative())return false;
    const auto at=[&](std::uint32_t value)noexcept {
        pc=sourcePostParticleModelsPC_=sourceWaterProvider274720_.pc=sourceWaterProviderNumeric_.pc=r.pc=value;
    };
    h.scalarTailEntered=true;h.opacitySubtractAttempted=true;
    std::uint32_t difference=0,clearMask=0;
    if(!g::detail::sourceWaterOpacitySubtract50LA(*h.maxRead2B9408,*h.opacityReload2B9418,difference,clearMask))return false;
    h.opacityDifference=difference;h.opacitySubtractClearMask=clearMask;
    at(0x2b942c);at(0x2b9430);
    if(!worldApplication_->waterLoadScalarAdmittedNative(6))return false;
    auto& process=worldApplication_->sourceWaterProcessScalars_;
    process.words[6]=h.opacityDifference;h.opacityRangePublication=++process.publications[6];
    //Six setup instructions between each successive call, including JAL's
    //destination delay slot. First call's arguments were set before SUB.S;
    //its delay slot above already published443ED8 before any getter reads.
    static constexpr std::array<std::uint32_t,9> calls{{0x2b942cu,0x2b9444u,0x2b945cu,0x2b9474u,
        0x2b948cu,0x2b94a4u,0x2b94bcu,0x2b94d4u,0x2b94ecu}};
    auto& config=*sourceWaterConfigOwner_;a::OriginalStandardTextureDiagnostic50LA d;
    for(std::size_t i=0;i<calls.size();++i){
        if(i)for(auto instruction=calls[i]-16u;instruction<=calls[i]+4u;instruction+=4u)at(instruction);
        auto& destination=i<4u?sourceWaterProvider_->channelScale98[i]:sourceWaterProvider_->framesPerSecond84[i-4u];
        if(!config.readWaterFloat158B80(5u+i,destination,d)){at(config.pc_);return false;}
    }
    at(0x2b94f4);at(0x2b94f8);at(0x2b94fc);at(0x2b9500);at(0x2b9504);at(0x2b9508);at(0x2b950c);at(0x2b9510);
    at(0x158b80);if(!worldApplication_->waterLoadScalarAdmittedNative(7))return false;
    if(!config.readWaterFloat158B80(14,process.words[7],d)){at(config.pc_);return false;}
    h.wakeScalePublication=++process.publications[7];
    at(0x2b9514);at(0x2b9518);at(0x2b951c);at(0x2b9520);at(0x2b9524);at(0x2b9528);
    return continueSourceWaterColors2B9528(pc);
}

#include "original_source_world_water_colors50la.inl"

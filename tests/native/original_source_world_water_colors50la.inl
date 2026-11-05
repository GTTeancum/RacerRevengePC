//Six original RGB integer getters, followed by five original float getters.
bool Owner::preflightSourceWaterColorsNative()const noexcept {
    if(!preflightSourceWaterFlagsNative())return false;
    const auto& h=sourceWaterConfig_;const auto& r=sourceWaterProviderInitializer_;
    if(!h.colorGettersEntered){
        for(const auto& p:h.colorPublications)if(p)return false;
        for(const auto& p:h.followingFloatPublications)if(p)return false;
        if(sourceWaterConfigOwner_){
            for(std::size_t i=0;i<6u;++i)if(sourceWaterConfigOwner_->integerGetters_[i].attempted)return false;
            for(std::size_t i=15;i<20u;++i)if(sourceWaterConfigOwner_->floatGetters_[i].attempted)return false;
        }
        return true;
    }
    if(!h.scalarTailEntered||!h.wakeScalePublication||!sourceWaterConfigOwner_||
       (!sourceWaterProvider274720_.nativeWithdrawn&&(!sourceWaterProvider_||!worldApplication_)))return false;
    const auto& config=*sourceWaterConfigOwner_;
    bool stopped=false;
    for(std::size_t i=0;i<11u;++i){
        const auto& receipt=i<6u?h.colorPublications[i]:h.followingFloatPublications[i-6u];
        const bool attempted=i<6u?config.integerGetters_[i].attempted:config.floatGetters_[i+9u].attempted;
        const auto returned=i<6u?config.integerGetters_[i].returned:config.floatGetters_[i+9u].returned;
        const auto result=i<6u?config.integerGetters_[i].result:config.floatGetters_[i+9u].result;
        if(stopped){if(attempted||receipt)return false;continue;}
        if(!returned){
            if(receipt||r.pc!=(attempted?config.pc_:(i<6u?0x158b30u:0x158b80u)))return false;
            stopped=true;continue;
        }
        if(!receipt||!*receipt)return false;
        if(!sourceWaterProvider274720_.nativeWithdrawn){
            const auto& process=worldApplication_->sourceWaterProcessScalars_;
            if(process.publications[8u+i]<*receipt||
               (process.publications[8u+i]==*receipt&&!process.unknown&&process.words[8u+i]!=result))return false;
        }
    }
    return stopped||h.flagGettersEntered||r.pc==0x2b9670u;
}
bool Owner::continueSourceWaterColors2B9528(std::uint32_t& pc)noexcept {
    auto& h=sourceWaterConfig_;auto& r=sourceWaterProviderInitializer_;
    if(pc!=0x2b9528u||r.pc!=pc||sourcePostParticleModelsPC_!=pc||h.colorGettersEntered||h.nativeWithdrawn||
       !sourceWaterConfigOwner_||!sourceWaterProvider_||!worldApplication_||!sourceLoadApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||nativeScbThread_!=std::this_thread::get_id()||
       !preflightSourceWaterConfigNative())return false;
    const auto at=[&](std::uint32_t value)noexcept {
        pc=sourcePostParticleModelsPC_=sourceWaterProvider274720_.pc=sourceWaterProviderNumeric_.pc=r.pc=value;
    };
    auto& config=*sourceWaterConfigOwner_;auto& process=worldApplication_->sourceWaterProcessScalars_;
    a::OriginalStandardTextureDiagnostic50LA d;h.colorGettersEntered=true;
    static constexpr std::array<std::uint32_t,6> calls{{0x2b9528u,0x2b9544u,0x2b9560u,0x2b957cu,0x2b9598u,0x2b95b4u}};
    for(std::size_t i=0;i<calls.size();++i){
        for(auto instruction=i?calls[i]-20u:calls[i]+4u;instruction<=calls[i]+4u;instruction+=4u)at(instruction);
        at(0x158b30);if(!worldApplication_->waterLoadScalarAdmittedNative(8u+i))return false;
        if(!config.readWaterInteger158B30(i,process.words[8u+i],d)){at(config.pc_);return false;}
        h.colorPublications[i]=++process.publications[8u+i]; //No fallible gap after actual source SW.
    }
    static constexpr std::array<std::uint32_t,5> starts{{0x2b95bcu,0x2b95e0u,0x2b9600u,0x2b9620u,0x2b963cu}};
    static constexpr std::array<std::uint32_t,5> floats{{0x2b95d8u,0x2b95f8u,0x2b9618u,0x2b9634u,0x2b9654u}};
    for(std::size_t i=0;i<floats.size();++i){
        for(auto instruction=starts[i];instruction<=floats[i]+4u;instruction+=4u)at(instruction);
        at(0x158b80);if(!worldApplication_->waterLoadScalarAdmittedNative(14u+i))return false;
        if(!config.readWaterFloat158B80(15u+i,process.words[14u+i],d)){at(config.pc_);return false;}
        h.followingFloatPublications[i]=++process.publications[14u+i];
    }
    at(0x2b965c);at(0x2b9660);at(0x2b9664);at(0x2b9668);at(0x2b966c);at(0x2b9670);
    return continueSourceWaterFlags2B9670(pc);
}

#include "original_source_world_water_flags50la.inl"

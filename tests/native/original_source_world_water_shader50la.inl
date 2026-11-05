//ShaderName and NumZones, then original live Env RGB loads/stores.
bool Owner::preflightSourceWaterShaderNative()const noexcept {
    if(!preflightSourceWaterAlphaNative())return false;
    const auto& h=sourceWaterConfig_;const auto& r=sourceWaterProviderInitializer_;
    const auto* config=sourceWaterConfigOwner_.get();
    if(sourceWaterProvider_){
        const auto expected=config?config->integerGetters_[11].result:std::optional<std::uint32_t>{};
        if(sourceWaterProvider_->numZones4A8!=expected)return false;
    }
    for(std::size_t i=0;i<h.shader140.value.size();++i){
        const auto* receipt=config&&config->shaderGetter_.defaultCopy.stores.known[i]?
            &config->shaderGetter_.defaultCopy.stores:
            config&&config->shaderGetter_.lookupCopy.stores.known[i]?&config->shaderGetter_.lookupCopy.stores:nullptr;
        if(h.shader140.known[i]!=bool(receipt)||
           (receipt&&h.shader140.value[i]!=receipt->value[i]))return false;
    }
    const auto noReads=[&](std::size_t start)noexcept {
        for(std::size_t i=start;i<3u;++i)if(h.envReads[i]||h.envReadPublications[i])return false;
        return true;
    };
    if(!h.shaderEntered)return noReads(0)&&(!config||(!config->shaderGetter_.attempted&&!config->integerGetters_[11].attempted));
    if(!h.flagGettersEntered||!config||!config->integerGetters_[10].returned||
       (!sourceWaterProvider274720_.nativeWithdrawn&&(!sourceWaterProvider_||!worldApplication_)))return false;
    if(!config->shaderGetter_.returned)return !config->integerGetters_[11].attempted&&noReads(0)&&
        r.pc==(config->shaderGetter_.attempted?config->pc_:0x158ad0u);
    const auto& zones=config->integerGetters_[11];
    if(!zones.returned)return zones.attempted&&noReads(0)&&r.pc==config->pc_;
    static constexpr std::array<std::uint32_t,3> reads{{0x2b9738u,0x2b9750u,0x2b975cu}};
    for(std::size_t i=0;i<3u;++i){
        if(bool(h.envReads[i])!=bool(h.envReadPublications[i]))return false;
        if(!h.envReads[i])return noReads(i+1u)&&r.pc==reads[i];
        if(!*h.envReadPublications[i]||sourceWaterProviderMembers_.integerArrays[0][i]!=h.envReads[i])return false;
        if(!sourceWaterProvider274720_.nativeWithdrawn){
            const auto& p=worldApplication_->sourceWaterProcessScalars_;
            if(p.publications[8u+i]<*h.envReadPublications[i]||
               (p.publications[8u+i]==*h.envReadPublications[i]&&!p.unknown&&p.words[8u+i]!=h.envReads[i]))return false;
        }
    }
    return h.alphaTailEntered||r.pc==0x2b9768u;
}
bool Owner::continueSourceWaterShader2B9714(std::uint32_t& pc)noexcept {
    auto& h=sourceWaterConfig_;auto& r=sourceWaterProviderInitializer_;
    if(pc!=0x2b9714u||r.pc!=pc||sourcePostParticleModelsPC_!=pc||h.shaderEntered||h.nativeWithdrawn||
       !sourceWaterConfigOwner_||!sourceWaterProvider_||!worldApplication_||!sourceLoadApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||nativeScbThread_!=std::this_thread::get_id()||
       !preflightSourceWaterConfigNative())return false;
    const auto at=[&](std::uint32_t value)noexcept {
        pc=sourcePostParticleModelsPC_=sourceWaterProvider274720_.pc=sourceWaterProviderNumeric_.pc=r.pc=value;
    };
    auto& config=*sourceWaterConfigOwner_;a::OriginalStandardTextureDiagnostic50LA d;
    h.shaderEntered=true;at(0x2b9718);at(0x158ad0);
    if(!config.readShaderName158AD0(h.shader140,d)){at(config.pc_);return false;}
    for(auto instruction=0x2b971cu;instruction<=0x2b9730u;instruction+=4u)at(instruction);
    if(!config.readWaterInteger158B30(11,sourceWaterProvider_->numZones4A8,d)){at(config.pc_);return false;}
    static constexpr std::array<std::uint32_t,3> reads{{0x2b9738u,0x2b9750u,0x2b975cu}};
    static constexpr std::array<std::uint32_t,3> stores{{0x2b9748u,0x2b9754u,0x2b9760u}};
    std::uint32_t next=0x2b9734u;
    for(std::size_t i=0;i<3u;++i){
        for(auto instruction=next;instruction<=reads[i];instruction+=4u)at(instruction);
        std::uint32_t value=0;std::uint64_t publication=0;
        if(!worldApplication_->waterScalarReadNative(8u+i,value,publication))return false;
        h.envReads[i]=value;h.envReadPublications[i]=publication;
        for(auto instruction=reads[i]+4u;instruction<=stores[i];instruction+=4u)at(instruction);
        sourceWaterProviderMembers_.integerArrays[0][i]=value; //The SAME constructor-owned color lane.
        next=stores[i]+4u;
    }
    at(0x2b9764);at(0x2b9768);
    return continueSourceWaterAlpha2B9768(pc);
}

#include "original_source_world_water_alpha50la.inl"

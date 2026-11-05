//Five original integer getters and four individual LodBias global stores.
bool Owner::preflightSourceWaterFlagsNative()const noexcept {
    if(!preflightSourceWaterShaderNative())return false;
    const auto& h=sourceWaterConfig_;const auto& r=sourceWaterProviderInitializer_;
    const auto* config=sourceWaterConfigOwner_.get();
    const auto result=[&](std::size_t i)noexcept {
        return config?config->integerGetters_[i].result:std::optional<std::uint32_t>{};
    };
    if(sourceWaterProvider_){
        auto signature=result(9);if(!signature&&sourceWaterProvider274720_.zeroWordsWritten>16u)signature=0u;
        if(sourceWaterProvider_->baseDisplacement49C!=result(7)||
           sourceWaterProvider_->signatureWaves6C!=signature||sourceWaterProvider_->numPools64!=result(10))return false;
    }
    if(h.lodBiasA44!=result(8))return false;
    const auto noLod=[&]()noexcept {
        if(h.lodRead2B96A8)return false;
        for(const auto& p:h.lodPublications)if(p)return false;
        return true;
    };
    if(!h.flagGettersEntered){
        if(h.renderInfinitePublication||!noLod())return false;
        if(config)for(std::size_t i=6;i<11u;++i)if(config->integerGetters_[i].attempted)return false;
        return true;
    }
    if(!h.colorGettersEntered||!h.followingFloatPublications[4]||!config||
       !config->floatGetters_[19].returned||
       (!sourceWaterProvider274720_.nativeWithdrawn&&(!sourceWaterProvider_||!worldApplication_)))return false;
    const auto& f=config->integerGetters_;
    const auto noLater=[&](std::size_t first)noexcept {
        for(std::size_t i=first;i<11u;++i)if(f[i].attempted)return false;
        return true;
    };
    const auto publication=[&](std::size_t lane,const std::optional<std::uint64_t>& receipt,
                               const std::optional<std::uint32_t>& value)noexcept {
        if(!receipt||!*receipt||!value)return false;
        if(sourceWaterProvider274720_.nativeWithdrawn)return true;
        const auto& p=worldApplication_->sourceWaterProcessScalars_;
        return p.publications[lane]>=*receipt&&
            (p.publications[lane]!=*receipt||p.unknown||p.words[lane]==value);
    };
    if(!f[6].returned)return !h.renderInfinitePublication&&noLod()&&noLater(7)&&
        r.pc==(f[6].attempted?config->pc_:0x158b30u);
    if(!publication(19,h.renderInfinitePublication,f[6].result))return false;
    if(!f[7].returned)return f[7].attempted&&noLod()&&noLater(8)&&r.pc==config->pc_;
    if(!f[8].returned)return f[8].attempted&&noLod()&&noLater(9)&&r.pc==config->pc_;
    if(!h.lodRead2B96A8||h.lodRead2B96A8!=h.lodBiasA44)return false;
    static constexpr std::array<std::uint32_t,4> stores{{0x2b96bcu,0x2b96c4u,0x2b96ccu,0x2b96e0u}};
    for(std::size_t i=0;i<4u;++i){
        if(!h.lodPublications[i]){
            for(std::size_t j=i+1u;j<4u;++j)if(h.lodPublications[j])return false;
            return noLater(9)&&r.pc==stores[i];
        }
        if(!publication(20u+i,h.lodPublications[i],h.lodRead2B96A8))return false;
    }
    if(!f[9].returned)return f[9].attempted&&noLater(10)&&r.pc==config->pc_;
    if(!f[10].returned)return f[10].attempted&&r.pc==config->pc_;
    return h.shaderEntered||r.pc==0x2b9714u; //Before the separate ShaderName158AD0 getter.
}
bool Owner::continueSourceWaterFlags2B9670(std::uint32_t& pc)noexcept {
    auto& h=sourceWaterConfig_;auto& r=sourceWaterProviderInitializer_;
    if(pc!=0x2b9670u||r.pc!=pc||sourcePostParticleModelsPC_!=pc||h.flagGettersEntered||h.nativeWithdrawn||
       !sourceWaterConfigOwner_||!sourceWaterProvider_||!worldApplication_||!sourceLoadApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||nativeScbThread_!=std::this_thread::get_id()||
       !preflightSourceWaterConfigNative())return false;
    const auto at=[&](std::uint32_t value)noexcept {
        pc=sourcePostParticleModelsPC_=sourceWaterProvider274720_.pc=sourceWaterProviderNumeric_.pc=r.pc=value;
    };
    auto& config=*sourceWaterConfigOwner_;auto& process=worldApplication_->sourceWaterProcessScalars_;
    a::OriginalStandardTextureDiagnostic50LA d;h.flagGettersEntered=true;
    at(0x2b9674);at(0x158b30);
    if(!worldApplication_->waterLoadScalarAdmittedNative(19))return false;
    if(!config.readWaterInteger158B30(6,process.words[19],d)){at(config.pc_);return false;}
    h.renderInfinitePublication=++process.publications[19];
    for(auto instruction=0x2b9678u;instruction<=0x2b968cu;instruction+=4u)at(instruction);
    if(!config.readWaterInteger158B30(7,sourceWaterProvider_->baseDisplacement49C,d)){at(config.pc_);return false;}
    for(auto instruction=0x2b9690u;instruction<=0x2b96a4u;instruction+=4u)at(instruction);
    if(!config.readWaterInteger158B30(8,h.lodBiasA44,d)){at(config.pc_);return false;}
    at(0x2b96a8);h.lodRead2B96A8=h.lodBiasA44;
    //Actual LW result is copied four times, not reread or recomputed from a
    //default. Each SW has its own shared-lane guard and historical receipt.
    static constexpr std::array<std::uint32_t,4> stores{{0x2b96bcu,0x2b96c4u,0x2b96ccu,0x2b96e0u}};
    std::uint32_t next=0x2b96acu;
    for(std::size_t i=0;i<4u;++i){
        for(auto instruction=next;instruction<=stores[i];instruction+=4u)at(instruction);
        if(!worldApplication_->waterLoadScalarAdmittedNative(20u+i))return false;
        process.words[20u+i]=h.lodRead2B96A8;h.lodPublications[i]=++process.publications[20u+i];
        next=stores[i]+4u;
    }
    //The fourth SW above is in JAL2B96DC's delay slot, BEFORE158B30 runs.
    if(!config.readWaterInteger158B30(9,sourceWaterProvider_->signatureWaves6C,d)){at(config.pc_);return false;}
    for(auto instruction=0x2b96e4u;instruction<=0x2b96f8u;instruction+=4u)at(instruction);
    if(!config.readWaterInteger158B30(10,sourceWaterProvider_->numPools64,d)){at(config.pc_);return false;}
    for(auto instruction=0x2b96fcu;instruction<=0x2b9714u;instruction+=4u)at(instruction);
    return continueSourceWaterShader2B9714(pc);
}

#include "original_source_world_water_shader50la.inl"

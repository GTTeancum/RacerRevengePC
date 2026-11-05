//Same-App startup alpha, current Froth words, then two complete current LQs.
bool Owner::preflightSourceWaterAlphaNative()const noexcept {
    if(!preflightSourceWaterWakeNative())return false;
    const auto& h=sourceWaterConfig_;const auto& r=sourceWaterProviderInitializer_;
    const auto noColors=[&](std::size_t start)noexcept {
        for(std::size_t i=start;i<5u;++i)if(h.colorTailReads[i]||h.colorTailReadPublications[i])return false;
        return true;
    };
    const auto noVectors=[&](std::size_t start)noexcept {
        if(h.wakeFrothCallIssued)return false;
        for(std::size_t i=start;i<2u;++i)
            if(h.waterVectorReads[i]||h.waterVectorReadPublications[i]||h.waterVectorCopied[i])return false;
        return true;
    };
    if(!h.alphaTailEntered)return noColors(0)&&noVectors(0);
    if(!h.shaderEntered||!h.envReadPublications[2]||!sourceWaterConfigOwner_||
       !sourceWaterConfigOwner_->integerGetters_[11].returned||
       (!sourceWaterProvider274720_.nativeWithdrawn&&(!sourceWaterProvider_||!worldApplication_)))return false;
    const auto historical=[&](std::size_t lane,std::uint32_t value,std::uint64_t publication)noexcept {
        if(!publication)return false;
        if(sourceWaterProvider274720_.nativeWithdrawn)return true;
        const auto& p=worldApplication_->sourceWaterProcessScalars_;
        return p.publications[lane]>=publication&&
            (p.publications[lane]!=publication||p.unknown||p.words[lane]==value);
    };
    static constexpr std::array<std::size_t,5> lanes{{24u,11u,12u,13u,25u}};
    static constexpr std::array<std::uint32_t,5> reads{{0x2b9768u,0x2b9774u,0x2b9780u,0x2b978cu,0x2b9798u}};
    for(std::size_t i=0;i<5u;++i){
        if(bool(h.colorTailReads[i])!=bool(h.colorTailReadPublications[i]))return false;
        if(!h.colorTailReads[i])return noColors(i+1u)&&noVectors(0)&&r.pc==reads[i];
        const auto& destination=sourceWaterProviderMembers_.integerArrays[i?1u:0u][i?i-1u:3u];
        if(destination!=h.colorTailReads[i]||
           !historical(lanes[i],*h.colorTailReads[i],*h.colorTailReadPublications[i]))return false;
    }
    for(std::size_t i=0;i<2u;++i){
        if(bool(h.waterVectorReads[i])!=bool(h.waterVectorReadPublications[i])||
           bool(h.waterVectorReads[i])!=h.waterVectorCopied[i])return false;
        if(!h.waterVectorReads[i])return noVectors(i+1u)&&r.pc==(i?0x2b97b0u:0x2b97a4u);
        if(sourceWaterProviderMembers_.vectors[1u+i]!=h.waterVectorReads[i])return false;
        const auto& vectorLanes=g::OriginalApplicationEntities50LA::waterVectorLanesNative[i];
        for(std::size_t word=0;word<4u;++word)
            if(!historical(vectorLanes[word],(*h.waterVectorReads[i])[word],(*h.waterVectorReadPublications[i])[word]))return false;
    }
    return h.wakeFrothCallIssued&&(h.wakeSelectionEntered||r.pc==0x157d00u);
}
bool Owner::continueSourceWaterAlpha2B9768(std::uint32_t& pc)noexcept {
    auto& h=sourceWaterConfig_;auto& r=sourceWaterProviderInitializer_;
    if(pc!=0x2b9768u||r.pc!=pc||sourcePostParticleModelsPC_!=pc||h.alphaTailEntered||h.nativeWithdrawn||
       !sourceWaterConfigOwner_||!sourceWaterProvider_||!worldApplication_||!sourceLoadApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||nativeScbThread_!=std::this_thread::get_id()||
       !preflightSourceWaterConfigNative())return false;
    const auto at=[&](std::uint32_t value)noexcept {
        pc=sourcePostParticleModelsPC_=sourceWaterProvider274720_.pc=sourceWaterProviderNumeric_.pc=r.pc=value;
    };
    h.alphaTailEntered=true;
    static constexpr std::array<std::size_t,5> lanes{{24u,11u,12u,13u,25u}};
    static constexpr std::array<std::uint32_t,5> reads{{0x2b9768u,0x2b9774u,0x2b9780u,0x2b978cu,0x2b9798u}};
    static constexpr std::array<std::uint32_t,5> stores{{0x2b976cu,0x2b9778u,0x2b9784u,0x2b9790u,0x2b979cu}};
    std::uint32_t next=0x2b9768u;
    for(std::size_t i=0;i<5u;++i){
        for(auto instruction=next;instruction<=reads[i];instruction+=4u)at(instruction);
        std::uint32_t value=0;std::uint64_t publication=0;
        if(!worldApplication_->waterScalarReadNative(lanes[i],value,publication))return false;
        h.colorTailReads[i]=value;h.colorTailReadPublications[i]=publication;
        at(stores[i]);sourceWaterProviderMembers_.integerArrays[i?1u:0u][i?i-1u:3u]=value;
        next=stores[i]+4u;
    }
    for(std::size_t i=0;i<2u;++i){
        at(i?0x2b97acu:0x2b97a0u);at(i?0x2b97b0u:0x2b97a4u);
        std::array<std::uint32_t,4> words;std::array<std::uint64_t,4> publications;
        if(!worldApplication_->waterVectorReadNative(i,words,publications))return false;
        h.waterVectorReads[i]=words;h.waterVectorReadPublications[i]=publications;
        if(i){at(0x2b97b4);h.wakeFrothCallIssued=true;}
        at(i?0x2b97b8u:0x2b97a8u);
        sourceWaterProviderMembers_.vectors[1u+i]=words;h.waterVectorCopied[i]=true;
    }
    //2B97B8 SQ is the second157D00 call's delay slot and has already run.
    //Same Config, literal Wake Froth. General Information is still selected;
    //its teardown and the new section lookup have not been represented yet.
    at(0x157d00);return continueSourceWaterWake157D00(pc);
}

#include "original_source_world_water_wake50la.inl"

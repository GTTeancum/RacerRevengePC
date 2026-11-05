//Actual158AD0 ShaderName occurrence. Copy admission does not guess guest SP.
std::size_t OriginalWaterSourceConfig50LA::shaderScratchReadQuantumNative(std::size_t begin)noexcept {
    //A=loaderSP-30-10B0+890+begin; D=loaderSP+140.
    //Filename D=loaderSP+790 instead gives begin-FE0. Both differences
    //are congruent to begin modulo16 for EVERY loaderSP.
    //Not divisible8 forces byte mode. 8mod16 excludes LQ but allows LD.
    return (begin&7u)?1u:(begin&15u)?8u:16u;
}
OriginalWaterSourceConfig50LA::Text::Bytes OriginalWaterSourceConfig50LA::shaderLiteralNative(bool fallback)noexcept {
    Text::Bytes bytes;
    //Exact immutable read extents, including padding:3C0A68[16] or3A6288[8].
    static constexpr std::array<std::uint8_t,16> waterDefault{{
        0x57,0x61,0x74,0x65,0x72,0x44,0x65,0x66,0x61,0x75,0x6c,0x74,0,0,0,0}};
    for(std::size_t i=0;i<(fallback?16u:8u);++i)bytes.put(i,fallback?waterDefault[i]:0u);
    return bytes;
}
bool OriginalWaterSourceConfig50LA::copyShader309230(ShaderCopy309230& c,const Text::Bytes& source,
    std::size_t begin,std::size_t length,std::size_t quantum,ShaderBytes& destination)noexcept {
    auto& flowPC=fieldFlowPCNative();
    flowPC=0x309230;c.admissionAttempted=true;c.sourceBegin=begin;c.length=length;c.readQuantum=quantum;
    //Conservative union of possible byte/LD8/LQ16 extents using proved
    //relative congruence, never a guessed absolute stack alignment. Immutable
    //literals are8-not16-aligned;8 covers all destination congruences.
    //This is native whole-copy admission BEFORE helper entry, not source reads.
    if((quantum!=1u&&quantum!=8u&&quantum!=16u)||length>=destination.value.size())return false;
    c.requiredKnown=((length+1u+quantum-1u)/quantum)*quantum;
    if(begin>source.value.size()||c.requiredKnown>source.value.size()-begin)return false;
    for(std::size_t i=0;i<c.requiredKnown;++i)if(!source.known[begin+i])return false;
    for(std::size_t i=0;i<length;++i)if(!source.value[begin+i])return false;
    if(source.value[begin+length])return false;
    c.admitted=true;
    //All original309230 alignment branches have this exact byte effect and
    //return the supplied destination. No fallible native step remains. Extra
    //lookahead bytes are NOT copied, nor claimed as actual reads on every path.
    for(std::size_t i=0;i<=length;++i){
        destination.put(i,source.value[begin+i]);c.stores.put(i,source.value[begin+i]);
    }
    c.returned=true;flowPC=0x309340;return true;
}
bool OriginalWaterSourceConfig50LA::preflightShaderCopyNative(const ShaderCopy309230& c,
    const Text::Bytes& source,std::size_t begin,std::size_t length,std::size_t quantum)const noexcept {
    if(!c.admissionAttempted){
        if(c.admitted||c.returned||c.sourceBegin||c.length||c.readQuantum||c.requiredKnown)return false;
        for(const auto known:c.stores.known)if(known)return false;
        return true;
    }
    if((quantum!=1u&&quantum!=8u&&quantum!=16u)||length>=c.stores.value.size()||
       c.sourceBegin!=begin||c.length!=length||c.readQuantum!=quantum||
       c.requiredKnown!=((length+quantum)/quantum)*quantum||c.returned!=c.admitted)return false;
    for(std::size_t i=0;i<c.stores.value.size();++i){
        if(c.stores.known[i]!=(c.returned&&i<=length))return false;
        if(c.stores.known[i]&&(begin+i>=source.value.size()||!source.known[begin+i]||
           c.stores.value[i]!=source.value[begin+i]||(!c.stores.value[i]!=(i==length))))return false;
    }
    if(c.admitted){
        if(begin>source.value.size()||c.requiredKnown>source.value.size()-begin)return false;
        for(std::size_t i=0;i<c.requiredKnown;++i)if(!source.known[begin+i])return false;
    }
    return true;
}
bool OriginalWaterSourceConfig50LA::readShaderName158AD0(ShaderBytes& destination,D& d)noexcept {
    d={};if(shaderGetter_.attempted||!integerGetters_[10].returned||!sectionReturn_||
       nativeClosed_||!borrowed_||!preflightNative(waterReceiver_))return fail(d,E::phase,0x158ad0);
    auto& f=shaderGetter_;f.attempted=true;pc_=0x158aec;
    if(!findWaterField158240(f,"ShaderName",&destination,&f.lookupCopy))return false;
    if(!*f.found){
        pc_=0x158b08;const auto fallback=shaderLiteralNative(true);
        if(!copyShader309230(f.defaultCopy,fallback,0,12,8,destination))return false;
        pc_=0x158b10;
    }else pc_=0x158b00;
    f.returned=*f.found;pc_=0x158b24;return true;
}
bool OriginalWaterSourceConfig50LA::preflightShaderGetterNative()const noexcept {
    if(shaderGetter_.attempted&&(!integerGetters_[10].returned||!sectionReturn_))return false;
    return preflightShaderValueNative(shaderGetter_,sectionCounted_,shaderLiteralNative(true),12u,256u);
}
bool OriginalWaterSourceConfig50LA::preflightShaderValueNative(const ShaderGetter& f,std::uint32_t count,
    const Text::Bytes& fallback,std::size_t fallbackLength,std::size_t capacity)const noexcept {
    if(!capacity||capacity>256u)return false;
    const auto empty=shaderLiteralNative(false);
    const auto& lookupSource=f.valueBegin?f.scratch:empty;
    if(!preflightShaderCopyNative(f.lookupCopy,lookupSource,f.valueBegin.value_or(0),
        f.valueLength.value_or(0),f.valueBegin?shaderScratchReadQuantumNative(*f.valueBegin):8u)||
       !preflightShaderCopyNative(f.defaultCopy,fallback,0,fallbackLength,8))return false;
    if(!f.attempted){
        return !f.lookupReturned&&!f.found&&!f.row&&!f.tokenNext&&!f.keyToken&&!f.valueToken&&!f.valueBegin&&
            !f.valueLength&&!f.tokenCurrent&&!f.keyLength&&!f.outputKnown&&!f.returned&&
            !f.lookupCopy.admissionAttempted&&!f.defaultCopy.admissionAttempted;
    }
    if(f.row>count||
       f.lookupReturned!=bool(f.found)||f.lookupReturned!=f.lookupCopy.returned||
       (f.returned&&f.returned!=f.found)||
       (f.defaultCopy.admissionAttempted&&f.found!=false))return false;
    if(f.lookupCopy.admissionAttempted&&f.lookupCopy.length>=capacity)return false;
    if(f.lookupReturned){
        if(!f.outputKnown||f.outputKnown>capacity||!f.output.known[f.outputKnown-1u]||f.output.value[f.outputKnown-1u])return false;
        for(std::size_t i=0;i<f.outputKnown;++i)
            if(!f.output.known[i]||!f.lookupCopy.stores.known[i]||f.output.value[i]!=f.lookupCopy.stores.value[i])return false;
        if(*f.found){
            if(!f.valueBegin||!f.valueLength||*f.valueLength+1u!=f.outputKnown||f.defaultCopy.admissionAttempted)return false;
        }else if(f.outputKnown!=1u||f.valueBegin||f.valueLength)return false;
    }else if(f.outputKnown||f.returned||f.defaultCopy.admissionAttempted)return false;
    if(f.returned&&(!f.lookupCopy.returned||(!*f.returned&&!f.defaultCopy.returned)))return false;
    return true;
}

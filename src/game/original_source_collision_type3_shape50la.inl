//Distinct raw source11C710(1,NULL,NULL) prefix. No semantic/query projection.
    enum class SourceShapeStatus:std::uint8_t {owner,bounded};
    struct SourceShapeResult {SourceShapeStatus status;std::uint32_t sourcePC;};
    struct SourceShapeBlock {
        std::unique_ptr<std::uint8_t[]> raw;
        const void* rawIdentity=nullptr;
        const void* payload=nullptr;
        std::array<std::uint8_t,48> known{};
        std::size_t bytes=0;
        bool allocationReturned=false,constructorReturned=false,published=false;
    };
    struct SourceShapePrefix {
        const void* occurrence=nullptr;
        const OriginalWreckComponent50LA* entity=nullptr;
        const std::array<std::uint32_t,4>* sentinel=nullptr;
        std::optional<const void*>* actual8C=nullptr;
        std::optional<const void*>* actual90=nullptr;
        std::uint32_t* actual88=nullptr;
        bool attempted=false,nativeWithdrawn=false,cleanupReturned=false,allocationReturned=false;
        bool vectorsStored=false,published8C=false,typeStored=false,countStored=false,normalStored=false;
        bool boundaryReached=false;
        std::unique_ptr<std::uint8_t[]> raw;
        const void* shapeIdentity=nullptr;
        std::array<std::uint8_t,0x50> known{};
        std::array<SourceShapeBlock,7> blocks;
        std::array<bool,2> metadataStored{},sentinelRead{},endpointStored{};
        std::array<std::array<std::uint32_t,4>,2> sentinelWords{};
        unsigned loopIndex=0;
        std::uint32_t pc=0;
    } sourceShape_;
    static void sourceShapeWord(std::uint8_t* raw,std::uint8_t* known,std::size_t offset,std::uint32_t word)noexcept {
        std::memcpy(raw+offset,&word,4);for(unsigned j=0;j<4;++j)known[offset+j]=1;
    }
    static void sourceShapeQuad(std::uint8_t* raw,std::uint8_t* known,std::size_t offset,
        const std::array<std::uint32_t,4>& words)noexcept {
        std::memcpy(raw+offset,words.data(),16);for(unsigned j=0;j<16;++j)known[offset+j]=1;
    }
    SourceShapeResult beginSourceShape11C710(const void* occurrence,const OriginalWreckComponent50LA& actualEntity,
        const std::array<std::uint32_t,4>& actualSentinel,std::uint32_t actualCount,
        const void* actualA2,const void* actualA3,std::optional<const void*>& actual8C,
        std::uint32_t& actual88,std::optional<const void*>& actual90)noexcept {
        auto& s=sourceShape_;
        if(!occurrence||s.attempted||attempted_||retired_||actualCount!=1||actualA2||actualA3||
           !actual8C||*actual8C||!actual90||*actual90||&actual8C==&actual90||actual88||
           actualEntity.kind!=OriginalWreckComponentKind50LA::collision11B190||actualEntity.reference00!=1)
            return {SourceShapeStatus::owner,0x11c710u};
        s.attempted=true;attempted_=true;s.occurrence=occurrence;s.entity=&actualEntity;s.sentinel=&actualSentinel;
        s.actual8C=&actual8C;s.actual88=&actual88;s.actual90=&actual90;s.pc=0x11b900u;
        //Original fresh11B600 follows both genuine constructorNULL skips.
        actual8C=nullptr;s.pc=0x11b904u;actual90=nullptr;s.cleanupReturned=true;s.pc=0x11c740u;
        try{s.raw.reset(new std::uint8_t[0x50]);}catch(...){s.raw.reset();}
        s.allocationReturned=true;s.shapeIdentity=s.raw.get();
        const std::array<std::uint32_t,4> vf0{{0,0,0,0x3f800000u}};
        if(s.raw){
            s.pc=0x11c754u;sourceShapeQuad(s.raw.get(),s.known.data(),0x20,vf0);
            s.pc=0x11c75cu;sourceShapeQuad(s.raw.get(),s.known.data(),0x30,vf0);s.vectorsStored=true;
        }
        s.pc=0x11c760u;actual8C=s.shapeIdentity;s.published8C=true;
        s.pc=0x11c768u;actual88=3;s.typeStored=true;
        s.pc=0x11c778u; //Count store is the first allocator's delay slot, before its allocation.
        if(!s.raw){boundary_=s.pc;return {SourceShapeStatus::bounded,s.pc};}
        sourceShapeWord(s.raw.get(),s.known.data(),0,1);s.countStored=true;
        constexpr std::array<std::size_t,7> sizes{{8,48,32,32,4,32,32}};
        constexpr std::array<unsigned,7> counts{{0,2,1,1,0,1,1}};
        constexpr std::array<std::uint32_t,7> calls{{0x11c774,0x11c78c,0x11c7bc,0x11c7ec,0x11c818,0x11c82c,0x11c85c}};
        constexpr std::array<std::uint32_t,7> stores{{0x11c790,0x11c7c0,0x11c7f0,0x11c81c,0x11c830,0x11c860,0x11c88c}};
        for(unsigned i=0;i<7;++i){
            auto& block=s.blocks[i];s.pc=calls[i];block.bytes=sizes[i];
            try{block.raw.reset(new std::uint8_t[block.bytes]);}catch(...){block.raw.reset();}
            block.allocationReturned=true;block.rawIdentity=block.raw.get();
            if(counts[i]){
                //2E8990 NULL returns untouched; otherwise cookie then unsigned
                //1048E0 constructor iterations. Cookie8..15 stays unknown.
                if(block.raw){
                    sourceShapeWord(block.raw.get(),block.known.data(),0,16);
                    sourceShapeWord(block.raw.get(),block.known.data(),4,counts[i]);
                    for(unsigned j=0;j<counts[i];++j)sourceShapeQuad(block.raw.get(),block.known.data(),16+16*j,vf0);
                    block.payload=block.raw.get()+16;
                }
                block.constructorReturned=true;
            }else block.payload=block.raw.get();
            s.pc=stores[i];block.published=true; //Typed raw shape04..1C, no host pointerbits.
        }
        s.pc=0x11c898u;sourceShapeWord(s.raw.get(),s.known.data(),0x44,0);s.normalStored=true;
        //Original signed loop bound is2*actualCount (2), separate from unsigned ctor counts.
        for(;s.loopIndex<2;){
            const auto i=s.loopIndex;auto& meta=s.blocks[0];auto& endpoints=s.blocks[1];
            s.pc=0x11c8d4u;
            if(!meta.raw){boundary_=s.pc;return {SourceShapeStatus::bounded,s.pc};}
            sourceShapeWord(meta.raw.get(),meta.known.data(),4*i,0);s.metadataStored[i]=true;
            s.pc=0x11c908u;s.sentinelWords[i]=actualSentinel;s.sentinelRead[i]=true;
            s.pc=0x11c918u;
            if(!endpoints.payload){boundary_=s.pc;return {SourceShapeStatus::bounded,s.pc};}
            sourceShapeQuad(endpoints.raw.get(),endpoints.known.data(),16+16*i,s.sentinelWords[i]);s.endpointStored[i]=true;
            s.pc=0x11c920u;++s.loopIndex;
        }
        s.pc=0x11c934u;s.boundaryReached=true;boundary_=s.pc;
        return {SourceShapeStatus::bounded,s.pc}; //Next nullable result0x80, not11C710 return.
    }
    bool preflightSourceShapeNative()const noexcept {
        const auto& s=sourceShape_;
        if(!preflightSourceResultPrefixNative())return false;
        if(!s.attempted)return !s.occurrence&&!s.entity&&!s.sentinel&&!s.actual8C&&!s.actual88&&!s.actual90&&
            !s.nativeWithdrawn&&!s.cleanupReturned&&!s.allocationReturned&&!s.raw&&!s.shapeIdentity;
        if(!s.occurrence||!attempted_||loaded_||freshWreck_||shape8C_||result90_||preparedByProducer_||
           hitsProduced_||fractionsProduced_||boundary_!=s.pc)return false;
        if(s.nativeWithdrawn){
            if(!retired_||s.raw||s.entity||s.sentinel||s.actual8C||s.actual88||s.actual90)return false;
            for(const auto& block:s.blocks)if(block.raw)return false;
            return true; //Retained opaque aliases/knownness never dereference freed storage.
        }
        if(retired_||!s.entity||s.entity->reference00!=1||s.entity->kind!=OriginalWreckComponentKind50LA::collision11B190||
           !s.sentinel||!s.actual8C||!s.actual88||!s.actual90||!s.cleanupReturned||!s.allocationReturned||
           !s.published8C||!s.typeStored||*s.actual8C!=std::optional<const void*>(s.shapeIdentity)||
           *s.actual90!=std::optional<const void*>(sourceResultPrefix_.published90&&!sourceResultPrefix_.nativeWithdrawn?sourceResultPrefix_.resultIdentity:nullptr)||*s.actual88!=3||s.shapeIdentity!=s.raw.get())return false;
        if(!s.raw){
            if(s.vectorsStored||s.countStored||s.normalStored||s.loopIndex||s.boundaryReached||s.pc!=0x11c778u)return false;
            for(auto x:s.known)if(x)return false;
            for(const auto& block:s.blocks)if(block.allocationReturned||block.raw||block.published)return false;
            return true;
        }
        if(!s.vectorsStored||!s.countStored||!s.normalStored||s.loopIndex>2)return false;
        const std::array<std::uint32_t,4> vf0{{0,0,0,0x3f800000u}};
        std::array<std::uint8_t,0x50> shapeExpected{},shapeKnown{};
        sourceShapeWord(shapeExpected.data(),shapeKnown.data(),0,1);
        sourceShapeQuad(shapeExpected.data(),shapeKnown.data(),0x20,vf0);
        sourceShapeQuad(shapeExpected.data(),shapeKnown.data(),0x30,vf0);
        sourceShapeWord(shapeExpected.data(),shapeKnown.data(),0x44,0);
        if(s.known!=shapeKnown)return false;
        for(unsigned i=0;i<0x50;++i)if(shapeKnown[i]&&s.raw[i]!=shapeExpected[i])return false;
        constexpr std::array<std::size_t,7> sizes{{8,48,32,32,4,32,32}};
        constexpr std::array<unsigned,7> counts{{0,2,1,1,0,1,1}};
        for(unsigned i=0;i<7;++i){
            const auto& block=s.blocks[i];std::array<std::uint8_t,48> expected{},known{};
            if(!block.allocationReturned||!block.published||block.bytes!=sizes[i]||
               block.constructorReturned!=bool(counts[i])||block.rawIdentity!=block.raw.get()||
               block.payload!=(block.raw?(counts[i]?static_cast<const void*>(block.raw.get()+16):block.raw.get()):nullptr))return false;
            if(block.raw&&counts[i]){
                sourceShapeWord(expected.data(),known.data(),0,16);sourceShapeWord(expected.data(),known.data(),4,counts[i]);
                for(unsigned j=0;j<counts[i];++j)sourceShapeQuad(expected.data(),known.data(),16+16*j,vf0);
            }
            if(i==0)for(unsigned j=0;j<2;++j)if(s.metadataStored[j]){
                if(!block.raw)return false;sourceShapeWord(expected.data(),known.data(),4*j,0);
            }
            if(i==1)for(unsigned j=0;j<2;++j)if(s.endpointStored[j]){
                if(!block.raw)return false;sourceShapeQuad(expected.data(),known.data(),16+16*j,s.sentinelWords[j]);
            }
            if(block.known!=known)return false;
            for(std::size_t j=0;j<block.bytes;++j)if(known[j]&&block.raw[j]!=expected[j])return false;
        }
        for(unsigned i=0;i<2;++i){
            if(s.endpointStored[i]&&(!s.sentinelRead[i]||!s.metadataStored[i]))return false;
            if(s.sentinelRead[i]!=s.metadataStored[i])return false;
            if(i<s.loopIndex){if(!s.endpointStored[i])return false;}
            else if(i>s.loopIndex&&(s.metadataStored[i]||s.sentinelRead[i]||s.endpointStored[i]))return false;
        }
        if(s.boundaryReached)return s.loopIndex==2&&s.pc==0x11c934u;
        if(s.loopIndex!=0||s.endpointStored[0])return false;
        if(!s.blocks[0].raw)return !s.metadataStored[0]&&!s.sentinelRead[0]&&s.pc==0x11c8d4u;
        return !s.blocks[1].payload&&s.metadataStored[0]&&s.sentinelRead[0]&&s.pc==0x11c918u;
    }
    bool withdrawSourceShapeNative(const void* occurrence)noexcept {
        auto& s=sourceShape_;if(!s.attempted)return true;
        if(s.occurrence!=occurrence||!preflightSourceShapeNative())return false;
        if(s.nativeWithdrawn)return true;
        if(!withdrawSourceResultPrefixNative(occurrence))return false;
        for(auto& block:s.blocks)block.raw.reset();s.raw.reset();
        *s.actual8C=nullptr; //Native alias withdrawal; original shape88=3 remains historical/current.
        s.actual8C=nullptr;s.actual88=nullptr;s.actual90=nullptr;s.entity=nullptr;s.sentinel=nullptr;
        s.nativeWithdrawn=true;retired_=true;return true;
    }

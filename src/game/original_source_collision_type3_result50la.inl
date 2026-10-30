//Original11C934 suffix: pending result is owned before actual90 publication.
    struct SourceResultPrefix {
        bool attempted=false,nativeWithdrawn=false,allocationReturned=false,vectorsStored=false,countStored=false;
        bool published90=false,returned11C710=false;
        const void* occurrence=nullptr;
        std::unique_ptr<std::uint8_t[]> raw;
        const void* resultIdentity=nullptr;
        std::array<std::uint8_t,0x80> known{};
        std::array<SourceShapeBlock,7> blocks;
        std::array<std::optional<std::uint32_t>,7> countReads5C;
        std::optional<const void*> returnPayload58;
        std::uint32_t pc=0;
    } sourceResultPrefix_;
    enum class SourceResultStatus:std::uint8_t {owner,bounded,reached};
    struct SourceResultReturn {SourceResultStatus status;std::uint32_t sourcePC;};
    SourceResultReturn continueSourceResult11C934(const void* occurrence)noexcept {
        auto& r=sourceResultPrefix_;auto& s=sourceShape_;
        if(!occurrence||r.attempted||s.occurrence!=occurrence||s.nativeWithdrawn||!s.boundaryReached||
           s.pc!=0x11c934u||!preflightSourceShapeNative())return {SourceResultStatus::owner,0x11c934u};
        r.attempted=true;r.occurrence=occurrence;r.pc=0x11c934u;
        try{r.raw.reset(new std::uint8_t[0x80]);}catch(...){r.raw.reset();}
        r.allocationReturned=true;r.resultIdentity=r.raw.get();
        const std::array<std::uint32_t,4> vf0{{0,0,0,0x3f800000u}};
        if(r.raw){
            //Actual2E8860 constructs two INLINE vectors, without an array cookie.
            r.pc=0x11c95cu;
            for(unsigned i=0;i<2;++i)sourceShapeQuad(r.raw.get(),r.known.data(),16*i,vf0);
            r.pc=0x11c968u;sourceShapeQuad(r.raw.get(),r.known.data(),0x20,vf0);
            r.pc=0x11c970u;sourceShapeQuad(r.raw.get(),r.known.data(),0x30,vf0);
            r.pc=0x11c978u;sourceShapeQuad(r.raw.get(),r.known.data(),0x40,vf0);r.vectorsStored=true;
        }
        r.pc=0x11c980u;
        if(!r.raw)return {SourceResultStatus::bounded,r.pc}; //NULL firstuse, before every resultarray.
        sourceShapeWord(r.raw.get(),r.known.data(),0x5c,1);r.countStored=true;
        constexpr std::array<std::size_t,7> sizes{{4,4,4,2,48,32,32}};
        constexpr std::array<unsigned,7> counts{{0,0,0,0,2,1,1}};
        constexpr std::array<unsigned,7> offsets{{0x64,0x68,0x6c,0x70,0x50,0x54,0x58}};
        constexpr std::array<std::uint32_t,7> reads{{0x11c988,0x11c998,0x11c9a8,0x11c9b8,0x11c9c8,0x11c9fc,0x11ca2c}};
        constexpr std::array<std::uint32_t,7> calls{{0x11c98c,0x11c99c,0x11c9ac,0x11c9bc,0x11c9d4,0x11ca04,0x11ca34}};
        constexpr std::array<std::uint32_t,7> stores{{0x11c994,0x11c9a4,0x11c9b4,0x11c9c4,0x11c9f8,0x11ca28,0x11ca58}};
        for(unsigned i=0;i<7;++i){
            r.pc=reads[i];std::uint32_t count;std::memcpy(&count,r.raw.get()+0x5c,4);r.countReads5C[i]=count;
            //Exact same stored count1; no generalized negative/overflow arithmetic admitted.
            auto& block=r.blocks[i];r.pc=calls[i];block.bytes=sizes[i];
            try{block.raw.reset(new std::uint8_t[block.bytes]);}catch(...){block.raw.reset();}
            block.allocationReturned=true;block.rawIdentity=block.raw.get();
            if(counts[i]){
                if(block.raw){
                    sourceShapeWord(block.raw.get(),block.known.data(),0,16);
                    sourceShapeWord(block.raw.get(),block.known.data(),4,counts[i]);
                    for(unsigned j=0;j<counts[i];++j)sourceShapeQuad(block.raw.get(),block.known.data(),16+16*j,vf0);
                    block.payload=block.raw.get()+16;
                }
                block.constructorReturned=true;
            }else block.payload=block.raw.get();
            r.pc=stores[i];block.published=true;
            //ActualNULL is known zero. NonNULL remains a typed alias, never host bits.
            if(!block.payload)sourceShapeWord(r.raw.get(),r.known.data(),offsets[i],0);
        }
        //Last2E8990's V0 survives subsequent stores/epilogue, even whenNULL.
        r.returnPayload58=r.blocks[6].payload;
        r.pc=0x11ca5cu;*s.actual90=r.resultIdentity;r.published90=true;
        r.pc=0x11ca80u;r.returned11C710=true;
        return {SourceResultStatus::reached,r.pc}; //Control return only; no boolean or outer-pointer V0.
    }
    bool preflightSourceResultPrefixNative()const noexcept {
        const auto& r=sourceResultPrefix_;const auto& s=sourceShape_;
        if(!r.attempted)return !r.nativeWithdrawn&&!r.occurrence&&!r.raw&&!r.resultIdentity&&!r.allocationReturned&&
            !r.vectorsStored&&!r.countStored&&!r.published90&&!r.returned11C710&&!r.returnPayload58&&!r.pc;
        if(!s.attempted||r.occurrence!=s.occurrence||!r.occurrence||!s.boundaryReached||s.pc!=0x11c934u||
           !r.allocationReturned||loaded_||freshWreck_||shape8C_||result90_||preparedByProducer_||hitsProduced_||fractionsProduced_)return false;
        if(r.nativeWithdrawn){
            if(r.raw)return false;
            for(const auto& block:r.blocks)if(block.raw)return false;
            return true; //Opaque historical payload and outer identities only.
        }
        if(s.nativeWithdrawn||retired_||!s.actual90||!s.raw||r.resultIdentity!=r.raw.get()||
           *s.actual90!=std::optional<const void*>(r.published90?r.resultIdentity:nullptr))return false;
        if(!r.raw){
            if(r.vectorsStored||r.countStored||r.published90||r.returned11C710||r.returnPayload58||r.pc!=0x11c980u)return false;
            for(auto x:r.known)if(x)return false;
            for(const auto& count:r.countReads5C)if(count)return false;
            for(const auto& block:r.blocks)if(block.allocationReturned||block.raw||block.published)return false;
            return true;
        }
        if(!r.vectorsStored||!r.countStored||!r.published90||!r.returned11C710||!r.returnPayload58||r.pc!=0x11ca80u)return false;
        const std::array<std::uint32_t,4> vf0{{0,0,0,0x3f800000u}};
        std::array<std::uint8_t,0x80> expected{},known{};
        for(unsigned i=0;i<5;++i)sourceShapeQuad(expected.data(),known.data(),16*i,vf0);
        sourceShapeWord(expected.data(),known.data(),0x5c,1);
        constexpr std::array<std::size_t,7> sizes{{4,4,4,2,48,32,32}};
        constexpr std::array<unsigned,7> counts{{0,0,0,0,2,1,1}};
        constexpr std::array<unsigned,7> offsets{{0x64,0x68,0x6c,0x70,0x50,0x54,0x58}};
        for(unsigned i=0;i<7;++i){
            const auto& block=r.blocks[i];std::array<std::uint8_t,48> blockExpected{},blockKnown{};
            if(r.countReads5C[i]!=std::optional<std::uint32_t>(1u)||!block.allocationReturned||!block.published||block.bytes!=sizes[i]||block.constructorReturned!=bool(counts[i])||
               block.rawIdentity!=block.raw.get()||block.payload!=(block.raw?(counts[i]?
               static_cast<const void*>(block.raw.get()+16):block.raw.get()):nullptr))return false;
            if(block.raw&&counts[i]){
                sourceShapeWord(blockExpected.data(),blockKnown.data(),0,16);sourceShapeWord(blockExpected.data(),blockKnown.data(),4,counts[i]);
                for(unsigned j=0;j<counts[i];++j)sourceShapeQuad(blockExpected.data(),blockKnown.data(),16+16*j,vf0);
            }
            if(block.known!=blockKnown)return false;
            for(std::size_t j=0;j<block.bytes;++j)if(blockKnown[j]&&block.raw[j]!=blockExpected[j])return false;
            if(!block.payload)sourceShapeWord(expected.data(),known.data(),offsets[i],0);
        }
        if(r.known!=known||*r.returnPayload58!=r.blocks[6].payload)return false;
        for(unsigned i=0;i<0x80;++i)if(known[i]&&r.raw[i]!=expected[i])return false;
        return true;
    }
    bool withdrawSourceResultPrefixNative(const void* occurrence)noexcept {
        auto& r=sourceResultPrefix_;if(!r.attempted)return true;
        if(r.occurrence!=occurrence||!preflightSourceResultPrefixNative())return false;
        if(r.nativeWithdrawn)return true;
        //Native cancellation of reached allocations, NOT source11B600's74 store.
        for(auto& block:r.blocks)block.raw.reset();r.raw.reset();
        if(r.published90)*sourceShape_.actual90=nullptr;
        r.nativeWithdrawn=true;return true;
    }

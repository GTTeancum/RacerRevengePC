//Private raw36-byte node/raw20-byte face suffix of actual125890. These are
//not CollisionMesh semantic records and do not create source resource125014.
    struct SourceRawRecords {
        std::unique_ptr<std::uint8_t[]> raw;
        std::vector<std::uint8_t> known;
        std::uint32_t count=0,stride=0;
        std::size_t bytes=0;
        bool metadataPrepared=false,allocationAttempted=false,published=false;
        std::optional<const void*> payload;
    };
    struct SourceNodeReceipt {
        bool centerStored=false,halfStored=false,radiusEntered=false,radiusReturned=false,completed=false;
        std::array<std::uint32_t,3> radiusInput{};
        std::uint32_t radiusBits=0;
    };
    struct SourceFaceReceipt {
        bool normalRead=false,normalStored=false,completed=false;
        std::array<SourcePrefixWord,3> input;
        std::array<std::uint16_t,3> temporary{};
        std::uint8_t convertedMask=0;
    };
    struct SourceNodes {
        bool attempted=false,nodeCountAdmitted=false,nodeLoopInitialized=false,nodesReturned=false;
        bool faceCountAdmitted=false,faceLoopInitialized=false,facesReturned=false,returned125890=false,nativeWithdrawn=false;
        std::uint32_t pc=0,nodeOrdinal=0,faceOrdinal=0;
        std::optional<std::uint16_t> root10C; //Only source SH low16, not a known fullword.
        SourceRawRecords nodes,faces;
        std::vector<SourceNodeReceipt> nodeReceipts;
        std::vector<SourceFaceReceipt> faceReceipts;
        std::vector<SourceArrayRead> reads;
        std::size_t readAttempts=0;
        std::array<SourcePrefixWord,3> scratch;
    } sourceNodes_;
    bool allocateSourceRecords(SourceRawRecords& block,std::uint32_t count,std::uint32_t stride,
        std::uint32_t pc)noexcept {
        sourceNodes_.pc=pc;block.count=count;block.stride=stride;block.bytes=std::size_t(count)*stride;
        try{block.known.assign(block.bytes,0);}catch(...){block.known.clear();return false;}
        block.metadataPrepared=true;block.allocationAttempted=true;
        try{block.raw=std::unique_ptr<std::uint8_t[]>(new std::uint8_t[block.bytes]{});}catch(...){block.raw.reset();}
        block.payload=block.raw.get();block.published=true;
        return true; //Even count0 issues allocation and publishes its actual outcome.
    }
    template<class ActualRead>
    bool readSourceNodeBytes(void* output,std::size_t width,std::uint32_t pc,
        std::size_t& done,ActualRead& read)noexcept {
        auto& n=sourceNodes_;n.pc=pc;done=0;
        if(n.readAttempts>=n.reads.size())return false;
        auto& r=n.reads[n.readAttempts++];r.pc=pc;r.requested=width;
        r.returned=read(output,width,done);r.done=done;
        return done<=width&&r.returned;
    }
    template<class ActualRead>
    bool readSourceNodeDirect(SourceRawRecords& block,std::size_t offset,std::size_t width,
        std::uint32_t pc,ActualRead& read)noexcept {
        sourceNodes_.pc=pc;
        //Native destination safety stops the original first NULL-use call,
        //without inventing source IO/cursor effects or a successful readzero.
        if(!block.raw)return false;
        std::size_t done=0;const bool returned=readSourceNodeBytes(block.raw.get()+offset,width,pc,done,read);
        if(done>width)return false;
        for(std::size_t i=0;i<done;++i)block.known[offset+i]=1;
        return returned;
    }
    template<class ActualRead>
    bool readSourceNodeScratch(unsigned lane,std::uint32_t pc,ActualRead& read)noexcept {
        auto& word=sourceNodes_.scratch[lane];std::size_t done=0;
        const bool returned=readSourceNodeBytes(word.bytes.data(),4,pc,done,read);
        if(done>4)return false;word.known=std::uint8_t(word.known|((1u<<done)-1u));return returned;
    }
    static void storeSourceRecordWord(SourceRawRecords& block,std::size_t offset,
        const SourcePrefixWord& word,std::size_t width=4)noexcept {
        for(std::size_t i=0;i<width;++i){block.raw[offset+i]=word.bytes[i];block.known[offset+i]=std::uint8_t((word.known>>i)&1u);}
    }
    template<class ActualRead>
    SourcePrefixResult continueSourceType0NodesFaces12602C(ActualRead&& read)noexcept {
        auto& n=sourceNodes_;const auto& a=sourceArrays_;const auto& s=sourcePrefix_;
        if(n.attempted||!a.normalsReturned||a.nativeWithdrawn||!preflightSourceType0PrefixNative()||
           (a.pc!=0x12602c&&a.pc!=0x126030))return {SourcePrefixStatus::owner,a.pc};
        n.attempted=true;n.pc=a.pc;n.scratch=a.floatVertexScratch; //Exact reused stackE8/EC/F0 history.
        if(s.nodesE0.known!=15)return {SourcePrefixStatus::bounded,n.pc};
        const auto nodeCount=s.nodesE0.bits();
        if(nodeCount>32767u||nodeCount>std::numeric_limits<std::uint32_t>::max()/36u)
            return {SourcePrefixStatus::bounded,n.pc}; //Existing native node limit, not source validation.
        try{n.nodeReceipts.resize(nodeCount);n.reads.resize(std::size_t(nodeCount)*9u);}
        catch(...){return {SourcePrefixStatus::bounded,n.pc};}
        n.nodeCountAdmitted=true;
        if(!allocateSourceRecords(n.nodes,nodeCount,36,0x126038))return {SourcePrefixStatus::bounded,n.pc};
        n.nodeLoopInitialized=true;
        for(std::uint32_t i=0;i<nodeCount;++i){
            const auto offset=std::size_t(i)*36u;auto& receipt=n.nodeReceipts[i];
            if(!readSourceNodeDirect(n.nodes,offset,4,0x126074,read))return {SourcePrefixStatus::bounded,n.pc};
            constexpr std::uint32_t centerPC[3]={0x126090,0x1260ac,0x1260c8};
            for(unsigned lane=0;lane<3;++lane)if(!readSourceNodeScratch(lane,centerPC[lane],read))
                return {SourcePrefixStatus::bounded,n.pc};
            for(unsigned lane=0;lane<3;++lane)storeSourceRecordWord(n.nodes,offset+12u+lane*4u,n.scratch[lane]);
            receipt.centerStored=true;
            constexpr std::uint32_t halfPC[3]={0x126128,0x126144,0x126160};
            for(unsigned lane=0;lane<3;++lane)if(!readSourceNodeScratch(lane,halfPC[lane],read))
                return {SourcePrefixStatus::bounded,n.pc};
            for(unsigned lane=0;lane<3;++lane)storeSourceRecordWord(n.nodes,offset+24u+lane*4u,n.scratch[lane]);
            receipt.halfStored=true;n.pc=0x1261b0;
            for(unsigned lane=0;lane<3;++lane){if(n.scratch[lane].known!=15)return {SourcePrefixStatus::bounded,n.pc};
                receipt.radiusInput[lane]=n.scratch[lane].bits();}
            receipt.radiusEntered=true;
            if(!SourceCollisionPayloadMath50LA::radius1261B0(receipt.radiusInput,receipt.radiusBits))
                return {SourcePrefixStatus::bounded,n.pc};
            SourcePrefixWord radius;radius.store(receipt.radiusBits);storeSourceRecordWord(n.nodes,offset+4,radius);
            receipt.radiusReturned=true;
            if(!readSourceNodeDirect(n.nodes,offset+8,2,0x126218,read)||
               !readSourceNodeDirect(n.nodes,offset+10,2,0x126234,read))return {SourcePrefixStatus::bounded,n.pc};
            receipt.completed=true;n.nodeOrdinal=i+1; //12623C, after both ignored-count returned reads.
        }
        n.nodesReturned=true;n.pc=0x126250;
        if(s.facesE4.known!=15)return {SourcePrefixStatus::bounded,n.pc};
        const auto faceCount=s.facesE4.bits();
        if(faceCount>32768u||faceCount>std::numeric_limits<std::uint32_t>::max()/20u)
            return {SourcePrefixStatus::bounded,n.pc};
        try{n.faceReceipts.resize(faceCount);n.reads.resize(std::size_t(nodeCount)*9u+std::size_t(faceCount)*7u);}
        catch(...){return {SourcePrefixStatus::bounded,n.pc};} //Prior nodes/IO remain owned.
        n.faceCountAdmitted=true;
        if(!allocateSourceRecords(n.faces,faceCount,20,0x12625c))return {SourcePrefixStatus::bounded,n.pc};
        n.faceLoopInitialized=true;
        for(std::uint32_t i=0;i<faceCount;++i){
            const auto offset=std::size_t(i)*20u;auto& receipt=n.faceReceipts[i];
            if(!readSourceNodeDirect(n.faces,offset+8,4,0x126298,read))return {SourcePrefixStatus::bounded,n.pc};
            constexpr std::uint32_t pcs[3]={0x1262b4,0x1262d0,0x1262ec};
            for(unsigned lane=0;lane<3;++lane)if(!readSourceNodeScratch(lane,pcs[lane],read))
                return {SourcePrefixStatus::bounded,n.pc};
            receipt.input=n.scratch;receipt.normalRead=true;
            constexpr std::uint32_t conversions[3]={0x126320,0x126348,0x126360};
            for(unsigned lane=0;lane<3;++lane){
                n.pc=conversions[lane];
                if(n.scratch[lane].known!=15||!SourceCollisionPayloadMath50LA::faceLane126320(
                    n.scratch[lane].bits(),receipt.temporary[lane]))return {SourcePrefixStatus::bounded,n.pc};
                receipt.convertedMask=std::uint8_t(receipt.convertedMask|(1u<<lane));
            }
            //All three actual temporarySH conversions precede any face store.
            for(unsigned lane=0;lane<3;++lane){SourcePrefixWord value;value.store(receipt.temporary[lane]);
                storeSourceRecordWord(n.faces,offset+12u+lane*2u,value,2);}
            receipt.normalStored=true;
            constexpr std::uint32_t indexPC[3]={0x1263a0,0x1263bc,0x1263d8};
            for(unsigned lane=0;lane<3;++lane)if(!readSourceNodeDirect(n.faces,offset+lane*2u,2,indexPC[lane],read))
                return {SourcePrefixStatus::bounded,n.pc};
            receipt.completed=true;n.faceOrdinal=i+1;
        }
        n.facesReturned=true;n.root10C=0;n.returned125890=true;n.pc=0x126410;
        return {SourcePrefixStatus::reached,n.pc}; //Control return only; original sets no V0=1.
    }
    static std::uint32_t sourceRecordBits(const SourceRawRecords& block,std::size_t offset,unsigned width=4)noexcept {
        std::uint32_t result=0;for(unsigned i=0;i<width;++i)result|=std::uint32_t(block.raw[offset+i])<<(8*i);return result;
    }
    static bool sourceRecordKnown(const SourceRawRecords& block,std::size_t offset,unsigned width)noexcept {
        if(!block.raw)return false;for(unsigned i=0;i<width;++i)if(!block.known[offset+i])return false;return true;
    }
    bool preflightSourceRecordsNative(const SourceRawRecords& block,std::uint32_t stride)const noexcept {
        if(!block.allocationAttempted)return !block.raw&&!block.metadataPrepared&&!block.published&&!block.payload&&block.known.empty();
        if(!block.metadataPrepared||!block.published||!block.payload||block.stride!=stride||
           block.bytes!=std::size_t(stride)*block.count||block.known.size()!=block.bytes||*block.payload!=block.raw.get())return false;
        for(const auto known:block.known)if(known>1||(!block.raw&&known))return false;
        return true;
    }
    bool preflightSourceType0NodesNative()const noexcept {
        const auto& n=sourceNodes_;const auto& s=sourcePrefix_;const auto& a=sourceArrays_;
        if(!n.attempted)return !n.nativeWithdrawn&&!n.nodeCountAdmitted&&!n.faceCountAdmitted&&!n.readAttempts&&
            !n.nodes.raw&&!n.faces.raw&&!n.returned125890&&!n.root10C;
        if(n.nativeWithdrawn)return !n.nodes.raw&&!n.faces.raw&&n.nodes.known.empty()&&n.faces.known.empty();
        if(!a.normalsReturned||a.nativeWithdrawn||s.nativeWithdrawn||!shape8C_||
           !preflightSourceRecordsNative(n.nodes,36)||!preflightSourceRecordsNative(n.faces,20)||
           n.readAttempts>n.reads.size()||n.nodeLoopInitialized!=n.nodes.published||n.faceLoopInitialized!=n.faces.published)return false;
        if(!n.nodeCountAdmitted)return !n.nodes.allocationAttempted&&!n.faces.allocationAttempted&&!n.readAttempts&&
            !n.nodesReturned&&!n.returned125890;
        if(s.nodesE0.known!=15||n.nodes.count!=s.nodesE0.bits()||n.nodes.count>32767u||
           n.nodeReceipts.size()!=n.nodes.count||n.nodeOrdinal>n.nodes.count)return false;
        const auto nodeReads=std::size_t(n.nodes.count)*9u;
        if(n.faceCountAdmitted){
            if(!n.nodesReturned||s.facesE4.known!=15||n.faces.count!=s.facesE4.bits()||n.faces.count>32768u||
               n.faceReceipts.size()!=n.faces.count||n.faceOrdinal>n.faces.count||
               n.reads.size()!=nodeReads+std::size_t(n.faces.count)*7u)return false;
        }else if(n.faces.allocationAttempted||n.faceOrdinal||n.facesReturned||n.readAttempts>nodeReads||n.reads.size()!=nodeReads)return false;
        constexpr std::uint32_t nodePC[9]={0x126074,0x126090,0x1260ac,0x1260c8,0x126128,0x126144,0x126160,0x126218,0x126234};
        constexpr std::uint32_t facePC[7]={0x126298,0x1262b4,0x1262d0,0x1262ec,0x1263a0,0x1263bc,0x1263d8};
        for(std::size_t i=0;i<n.reads.size();++i){const auto& r=n.reads[i];
            if(i<n.readAttempts){
                const bool node=i<nodeReads;const auto lane=node?i%9u:(i-nodeReads)%7u;
                const auto width=node?(lane<7?4u:2u):(lane<4?4u:2u);
                if(r.pc!=(node?nodePC[lane]:facePC[lane])||r.requested!=width||r.done>width||
                   (!r.returned&&i+1!=n.readAttempts))return false;
            }else if(r.pc||r.requested||r.done||r.returned)return false;
        }
        const auto returned=[&](std::size_t index)noexcept{return index<n.readAttempts&&n.reads[index].returned;};
        std::uint32_t completed=0;
        for(std::uint32_t i=0;i<n.nodes.count;++i){const auto& r=n.nodeReceipts[i];const auto first=std::size_t(i)*9u,offset=std::size_t(i)*36u;
            if(r.centerStored!=returned(first+3)||r.halfStored!=returned(first+6)||r.completed!=returned(first+8)||
               (r.halfStored&&!r.centerStored)||(r.radiusEntered&&!r.halfStored)||(r.radiusReturned&&!r.radiusEntered)||
               (r.completed&&!r.radiusReturned))return false;
            if(r.radiusEntered){
                if(!sourceRecordKnown(n.nodes,offset+24,12))return false;
                for(unsigned lane=0;lane<3;++lane)if(r.radiusInput[lane]!=sourceRecordBits(n.nodes,offset+24+4*lane))return false;
            }
            if(r.radiusReturned&&(!sourceRecordKnown(n.nodes,offset+4,4)||sourceRecordBits(n.nodes,offset+4)!=r.radiusBits))return false;
            if(r.completed){if(i!=completed)return false;++completed;}
        }
        if(n.nodeOrdinal!=completed||n.nodesReturned!=(n.nodeLoopInitialized&&completed==n.nodes.count))return false;
        completed=0;
        for(std::uint32_t i=0;i<n.faceReceipts.size();++i){const auto& r=n.faceReceipts[i];
            if(!n.faceCountAdmitted){if(r.normalRead||r.normalStored||r.completed||r.convertedMask)return false;continue;}
            const auto first=nodeReads+std::size_t(i)*7u,offset=std::size_t(i)*20u;
            if(r.normalRead!=returned(first+3)||r.completed!=returned(first+6)||
               (r.convertedMask!=0&&r.convertedMask!=1&&r.convertedMask!=3&&r.convertedMask!=7)||
               (r.convertedMask&&!r.normalRead)||r.normalStored!=(r.convertedMask==7)||
               (r.completed&&!r.normalStored))return false;
            for(unsigned lane=0;lane<3;++lane){
                if((r.convertedMask&(1u<<lane))&&r.input[lane].known!=15)return false;
                if(r.normalStored&&(!sourceRecordKnown(n.faces,offset+12+lane*2,2)||
                    sourceRecordBits(n.faces,offset+12+lane*2,2)!=r.temporary[lane]))return false;
            }
            if(n.faces.raw&&(n.faces.known[offset+6]||n.faces.known[offset+7]||n.faces.known[offset+18]||n.faces.known[offset+19]))return false;
            if(r.completed){if(i!=completed)return false;++completed;}
        }
        if(n.faceOrdinal!=completed||n.facesReturned!=(n.faceLoopInitialized&&completed==n.faces.count))return false;
        if(n.returned125890)return n.nodesReturned&&n.facesReturned&&n.root10C&&*n.root10C==0&&n.pc==0x126410;
        return !n.root10C;
    }
    bool withdrawSourceType0NodesNative()noexcept {
        auto& n=sourceNodes_;if(!n.attempted||n.nativeWithdrawn)return true;
        if((sourceResource_.attempted&&!sourceResource_.nativeWithdrawn)||!preflightSourceType0NodesNative())return false;
        n.nodes.raw.reset();n.faces.raw.reset();n.nodes.known.clear();n.faces.known.clear();n.nativeWithdrawn=true;
        return true; //Native owned-prefix cancellation only; no resource124/ref60/cache.
    }

//Private source array stage, same shape owner. Raw byte allocations and cookie
//payloads are not decoded CollisionMesh/native cache authority.
    struct SourceArrayRead {
        std::uint32_t pc=0;
        std::size_t requested=0,done=0;
        bool returned=false;
    };
    struct SourceRawArray {
        std::unique_ptr<std::uint8_t[]> raw;
        std::vector<std::uint8_t> known;
        std::size_t bytes=0;
        std::uint32_t stride=0,count=0;
        bool metadataPrepared=false,allocationAttempted=false,cookieStored=false,constructorsReturned=false,published=false;
        bool zeroConstructor=false;
        std::optional<const void*> payload;
    };
    struct SourceArrays {
        bool attempted=false,admitted=false,nativeWithdrawn=false;
        bool verticesReturned=false,presenceReturned=false,normalZeroStored=false;
        bool normalsReturned=false,floatGuardPending=false,floatGuardsReturned=false,nodeCountLoaded=false;
        bool vertexLoopInitialized=false,normalLoopInitialized=false;
        const OriginalApplicationEntities50LA* application=nullptr;
        std::array<SourcePrefixWord,3> guardScratch;
        std::array<std::uint64_t,3> widened{};
        std::array<std::uint32_t,3> classified{};
        std::uint8_t mathMask=0;
        std::array<OriginalCollisionNormalGuards50LA::Receipt,3> guards;
        std::uint32_t count=0,pc=0;
        std::optional<std::uint32_t> nodeCountRegister;
        std::vector<SourceArrayRead> reads;
        std::size_t readAttempts=0;
        std::uint32_t verticesStored=0,normalsStored=0,vertexOrdinal=0,normalOrdinal=0;
        std::array<SourcePrefixWord,3> packedVertexScratch,floatVertexScratch,packedNormalScratch;
        SourcePrefixWord presenceF8;
        SourceRawArray vertices,normals;
    } sourceArrays_;
    template<class ActualRead>
    bool readSourceArrayWord(SourcePrefixWord& destination,std::size_t width,
        std::uint32_t pc,ActualRead& read)noexcept {
        auto& a=sourceArrays_;a.pc=pc;
        if(a.readAttempts>=a.reads.size())return false;
        auto& r=a.reads[a.readAttempts++];r.pc=pc;r.requested=width;
        std::size_t done=0;r.returned=read(destination.bytes.data(),width,done);r.done=done;
        if(done>width||width>4)return false;
        destination.known=std::uint8_t(destination.known|((1u<<done)-1u));
        return r.returned; //Ignored original read count; native no-call refusal remains terminal.
    }
    bool allocateSourceRawArray(SourceRawArray& array,std::uint32_t stride,bool zero,
        std::uint32_t callPC)noexcept {
        auto& a=sourceArrays_;a.pc=callPC;
        array.stride=stride;array.count=a.count;array.bytes=std::size_t(stride)*a.count+16u;
        //Knownness backing is native admission BEFORE the actual nullable
        //allocation. Failure here cannot invent a source NULL publication.
        try{array.known.assign(array.bytes,0);}catch(...){array.known.clear();return false;}
        array.metadataPrepared=true;array.zeroConstructor=zero;array.allocationAttempted=true;
        try{array.raw=std::unique_ptr<std::uint8_t[]>(new std::uint8_t[array.bytes]{});}catch(...){array.raw.reset();}
        if(array.raw){
            for(unsigned i=0;i<4;++i){array.raw[i]=std::uint8_t(stride>>(8*i));
                array.raw[4+i]=std::uint8_t(a.count>>(8*i));array.known[i]=array.known[4+i]=1;}
            array.cookieStored=true; //2E89D0/D4; cookie8..15 remains source unknown.
            if(zero)for(std::size_t i=16;i<array.bytes;++i){array.raw[i]=0;array.known[i]=1;}
            array.constructorsReturned=true; //126430/11C090 zero,126420 no stores.
        }
        array.payload=array.raw?static_cast<const void*>(array.raw.get()+16):nullptr;
        array.published=true;return true; //NULL rawbase2E8990 returnsNULL before cookie.
    }
    bool storeSourceArrayElement(SourceRawArray& array,std::uint32_t index,
        const std::array<SourcePrefixWord,3>& scratch,std::size_t width,std::uint32_t pc)noexcept {
        sourceArrays_.pc=pc;
        if(!array.raw)return false; //Actual first store, AFTER all three reads.
        const auto offset=16u+std::size_t(index)*array.stride;
        for(unsigned lane=0;lane<3;++lane)for(unsigned byte=0;byte<width;++byte){
            array.raw[offset+lane*width+byte]=scratch[lane].bytes[byte];
            //A source unknown scratch byte overwrites even constructor-known0.
            array.known[offset+lane*width+byte]=std::uint8_t((scratch[lane].known>>byte)&1u);
        }
        return true;
    }
    template<class ActualRead>
    SourcePrefixResult continueSourceType0Arrays125AB4(const OriginalApplicationEntities50LA& actualApplication,ActualRead&& read)noexcept {
        auto& s=sourcePrefix_;auto& a=sourceArrays_;
        if(a.attempted||s.nativeWithdrawn||!s.scalarReturned||!preflightSourceType0PrefixNative()||
           !shape8C_||!s.packedStored||s.packedF4.known!=15)
            return {SourcePrefixStatus::owner,s.pc};
        a.attempted=true;a.application=&actualApplication;a.pc=s.pc;
        if(s.vertex11C.known!=15)return {SourcePrefixStatus::bounded,a.pc};
        const auto count=s.vertex11C.bits();
        //Existing native CollisionLimits48G default65535, not a source count
        //clamp. Signed importer and UNSIGNED constructor loops differ; reject
        //negative/oversized/overflowing admission before any source allocation.
        if(count>65535u||count>(std::numeric_limits<std::uint32_t>::max()-16u)/12u)
            return {SourcePrefixStatus::bounded,a.pc};
        a.count=count;
        try{a.reads.resize(std::size_t(count)*6u+1u);}catch(...){return {SourcePrefixStatus::bounded,a.pc};}
        a.admitted=true;const bool packed=s.packedF4.bits()!=0;
        const std::size_t width=packed?2u:4u;
        if(!allocateSourceRawArray(a.vertices,packed?6u:12u,true,packed?0x125ac4u:0x125bb0u))
            return {SourcePrefixStatus::bounded,a.pc};
        a.vertexLoopInitialized=true;
        for(std::uint32_t i=0;i<count;++i){
            auto& scratch=packed?a.packedVertexScratch:a.floatVertexScratch;
            if(packed)for(auto& lane:scratch){lane.bytes[0]=lane.bytes[1]=0;lane.known=3;}
            const std::uint32_t pcs[2][3]={{0x125bf4,0x125c10,0x125c2c},{0x125b14,0x125b30,0x125b4c}};
            for(unsigned lane=0;lane<3;++lane)
                if(!readSourceArrayWord(scratch[lane],width,pcs[packed?1:0][lane],read))
                    return {SourcePrefixStatus::bounded,a.pc};
            a.vertexOrdinal=i+1; //125B6C/125C60 precedes first destination store.
            if(!storeSourceArrayElement(a.vertices,i,scratch,width,packed?0x125b70u:0x125c68u))
                return {SourcePrefixStatus::bounded,a.pc};
            ++a.verticesStored;
        }
        a.verticesReturned=true;
        if(!readSourceArrayWord(a.presenceF8,4,0x125ca4,read))return {SourcePrefixStatus::bounded,a.pc};
        a.presenceReturned=true;a.pc=0x125cac;
        if(a.presenceF8.known!=15)return {SourcePrefixStatus::bounded,a.pc};
        if(!a.presenceF8.bits()){
            a.normals.payload=nullptr;a.normals.published=true;a.normalZeroStored=true;
            a.normalsReturned=true;a.pc=0x12602c;return {SourcePrefixStatus::bounded,a.pc};
        }
        if(!allocateSourceRawArray(a.normals,packed?6u:12u,!packed,packed?0x125cd4u:0x125db0u))
            return {SourcePrefixStatus::bounded,a.pc};
        if(!packed){
            //Validate LAST VERTEX E8/EC/F0 before any normal read. Keep a
            //separate snapshot because the same scratch is subsequently reused.
            a.floatGuardPending=true;a.guardScratch=a.floatVertexScratch;
            constexpr std::uint32_t conversions[3]={0x125dd8,0x125e5c,0x125ee0};
            for(unsigned lane=0;lane<3;++lane){
                a.pc=conversions[lane];
                if(a.guardScratch[lane].known!=15)return {SourcePrefixStatus::bounded,a.pc};
                a.widened[lane]=SourceCollisionMath50LA::widen311038(a.guardScratch[lane].bits());
                a.classified[lane]=SourceCollisionMath50LA::classify3022D0(a.widened[lane]);
                a.mathMask=std::uint8_t(a.mathMask|(1u<<lane));
                const auto guard=OriginalCollisionNormalGuards50LA::reachAfterClassifier3022D0(
                    lane,actualApplication,a.classified[lane],a.guards[lane]);
                if(a.guards[lane].s0Zero125F18)a.normalLoopInitialized=true;
                a.pc=guard.sourcePC;
                if(guard.status!=OriginalCollisionNormalGuards50LA::Status::returned)
                    return {SourcePrefixStatus::bounded,a.pc};
            }
            a.floatGuardsReturned=true;a.floatGuardPending=false;
        }else a.normalLoopInitialized=true; //125D00; float S0 reset belongs to actual Z guard.
        for(std::uint32_t i=0;i<count;++i){
            constexpr std::uint32_t packedPC[3]={0x125d18,0x125d34,0x125d50};
            constexpr std::uint32_t floatPC[3]={0x125f84,0x125fa0,0x125fbc};
            auto& scratch=packed?a.packedNormalScratch:a.floatVertexScratch;
            for(unsigned lane=0;lane<3;++lane)
                if(!readSourceArrayWord(scratch[lane],width,(packed?packedPC:floatPC)[lane],read))
                    return {SourcePrefixStatus::bounded,a.pc};
            a.normalOrdinal=i+1; //125D6C/125FF0 before first store, also on NULLpayload bound.
            if(!storeSourceArrayElement(a.normals,i,scratch,width,packed?0x125d74u:0x125ff8u))
                return {SourcePrefixStatus::bounded,a.pc};
            ++a.normalsStored;
        }
        a.normalsReturned=true;
        if(!packed){a.pc=0x12602c;return {SourcePrefixStatus::bounded,a.pc};}
        a.pc=0x125d9c; //Packed branch delay loads stackE0 BEFORE126030.
        if(s.nodesE0.known!=15)return {SourcePrefixStatus::bounded,a.pc};
        a.nodeCountRegister=s.nodesE0.bits();a.nodeCountLoaded=true;a.pc=0x126030;
        return {SourcePrefixStatus::bounded,a.pc}; //Node malloc36*N remains separate.
    }
    bool preflightSourceRawArrayNative(const SourceRawArray& r,bool allowNullStore=false)const noexcept {
        if(!r.allocationAttempted){
            if(r.raw||r.cookieStored||r.constructorsReturned||r.metadataPrepared)return false;
            return (!r.published&&!r.payload)||(allowNullStore&&r.published&&r.payload&&!*r.payload);
        }
        if(!r.metadataPrepared||!r.published||!r.payload||r.count!=sourceArrays_.count||
           (r.stride!=6&&r.stride!=12)||r.bytes!=std::size_t(r.stride)*r.count+16u||r.known.size()!=r.bytes||
           *r.payload!=(r.raw?static_cast<const void*>(r.raw.get()+16):nullptr))return false;
        if(r.cookieStored!=bool(r.raw)||r.constructorsReturned!=bool(r.raw))return false;
        for(const auto k:r.known)if(k>1)return false;
        if(r.raw){
            for(unsigned i=0;i<4;++i)if(!r.known[i]||!r.known[4+i]||
                r.raw[i]!=std::uint8_t(r.stride>>(8*i))||r.raw[4+i]!=std::uint8_t(r.count>>(8*i)))return false;
            for(unsigned i=8;i<16;++i)if(r.known[i])return false;
        }else for(const auto k:r.known)if(k)return false;
        return true;
    }
    bool preflightSourceType0ArraysNative()const noexcept {
        const auto& s=sourcePrefix_;const auto& a=sourceArrays_;
        if(!a.attempted)return !a.application&&!a.nativeWithdrawn&&!a.admitted&&!a.readAttempts&&!a.vertices.raw&&!a.normals.raw;
        if(a.nativeWithdrawn)return !a.vertices.raw&&!a.normals.raw&&a.vertices.known.empty()&&a.normals.known.empty();
        if(!a.application||!s.scalarReturned||s.nativeWithdrawn||!shape8C_||a.verticesStored>a.count||a.normalsStored>a.count||
           !preflightSourceRawArrayNative(a.vertices)||!preflightSourceRawArrayNative(a.normals,a.normalZeroStored))return false;
        if(!a.admitted)return !a.readAttempts&&!a.vertices.allocationAttempted&&!a.normals.allocationAttempted;
        if(s.vertex11C.known!=15||a.count!=s.vertex11C.bits()||a.count>65535u||
           a.reads.size()!=std::size_t(a.count)*6u+1u||a.readAttempts>a.reads.size())return false;
        const bool packed=s.packedF4.bits()!=0;const auto vertexReads=std::size_t(a.count)*3u;
        constexpr std::uint32_t packedVertexPC[3]={0x125b14,0x125b30,0x125b4c};
        constexpr std::uint32_t floatVertexPC[3]={0x125bf4,0x125c10,0x125c2c};
        constexpr std::uint32_t packedNormalPC[3]={0x125d18,0x125d34,0x125d50};
        constexpr std::uint32_t floatNormalPC[3]={0x125f84,0x125fa0,0x125fbc};
        for(std::size_t i=0;i<a.reads.size();++i){const auto& r=a.reads[i];
            if(i<a.readAttempts){
                const auto expectedPC=i<vertexReads?(packed?packedVertexPC[i%3]:floatVertexPC[i%3]):
                    i==vertexReads?0x125ca4u:(packed?packedNormalPC:floatNormalPC)[(i-vertexReads-1)%3];
                const std::size_t width=i==vertexReads?4u:packed?2u:4u;
                if(r.requested!=width||r.done>r.requested||r.pc!=expectedPC||
                   (!packed&&i>vertexReads&&!a.floatGuardsReturned)||(!r.returned&&i+1!=a.readAttempts))return false;
            }else if(r.pc||r.requested||r.done||r.returned)return false;
        }
        if(a.vertices.allocationAttempted&&(a.vertices.stride!=(packed?6u:12u)||!a.vertices.zeroConstructor))return false;
        if(a.normals.allocationAttempted&&(a.normals.stride!=(packed?6u:12u)||a.normals.zeroConstructor==packed))return false;
        std::uint32_t completedVertices=0,completedNormals=0;
        for(std::uint32_t i=0;i<a.count;++i){const auto last=std::size_t(i)*3u+2u;
            if(last<a.readAttempts&&a.reads[last].returned)++completedVertices;
            const auto normalLast=vertexReads+1u+last;
            if(normalLast<a.readAttempts&&a.reads[normalLast].returned)++completedNormals;
        }
        if(a.vertexOrdinal!=completedVertices||a.normalOrdinal!=completedNormals||
           a.verticesStored!=(a.vertices.raw?completedVertices:0u)||
           a.normalsStored!=(a.normals.raw?completedNormals:0u))return false;
        if(a.presenceReturned!=(vertexReads<a.readAttempts&&a.reads[vertexReads].returned))return false;
        if(a.verticesReturned&&(!a.vertices.published||a.verticesStored!=a.count))return false;
        if(a.presenceReturned&&!a.verticesReturned)return false;
        if(a.normalZeroStored&&(!a.presenceReturned||a.presenceF8.known!=15||a.presenceF8.bits()||
           a.normals.allocationAttempted||!a.normalsReturned||a.pc!=0x12602c))return false;
        if(a.normals.allocationAttempted&&(!a.presenceReturned||a.presenceF8.known!=15||!a.presenceF8.bits()))return false;
        if(a.floatGuardPending&&(packed||!a.normals.published||a.floatGuardsReturned))return false;
        if(a.floatGuardsReturned&&(packed||!a.normals.published||a.mathMask!=7||!a.normalLoopInitialized))return false;
        for(unsigned lane=0;lane<3;++lane){
            if(!OriginalCollisionNormalGuards50LA::historyCurrent(a.guards[lane],a.application))return false;
            if(a.mathMask&(1u<<lane)){
                if(a.guardScratch[lane].known!=15||a.widened[lane]!=SourceCollisionMath50LA::widen311038(a.guardScratch[lane].bits())||
                   a.classified[lane]!=SourceCollisionMath50LA::classify3022D0(a.widened[lane])||
                   !a.guards[lane].entered||a.guards[lane].classifier!=a.classified[lane]||
                   (lane&&!a.guards[lane-1].returned))return false;
            }else if(a.guards[lane].entered||a.widened[lane]||a.classified[lane])return false;
        }
        if(a.mathMask>7||(packed&&(a.mathMask||a.floatGuardPending||a.floatGuardsReturned)))return false;
        if(a.vertexLoopInitialized!=a.vertices.published||
           a.normalLoopInitialized!=(packed?(a.normals.allocationAttempted&&a.normals.published):a.guards[2].s0Zero125F18))return false;
        if(a.floatGuardPending){
            if(a.readAttempts!=vertexReads+1u)return false;
            constexpr std::uint32_t conversions[3]={0x125dd8,0x125e5c,0x125ee0};
            bool stopped=false;
            for(unsigned lane=0;lane<3;++lane){
                if(a.guardScratch[lane].bytes!=a.floatVertexScratch[lane].bytes||
                   a.guardScratch[lane].known!=a.floatVertexScratch[lane].known)return false;
                if(!stopped&&!a.guards[lane].returned){
                    if(a.pc!=((a.mathMask&(1u<<lane))?a.guards[lane].pc:conversions[lane]))return false;
                    stopped=true;
                }
            }
            if(!stopped)return false;
        }
        if(a.normalsReturned&&!a.normalZeroStored&&(a.normalsStored!=a.count||
            (packed?(a.pc!=0x125d9c&&a.pc!=0x126030):(a.pc!=0x12602c||!a.floatGuardsReturned))))return false;
        if(a.nodeCountLoaded&&(!a.normalsReturned||a.normalZeroStored||s.nodesE0.known!=15||
            a.nodeCountRegister!=s.nodesE0.bits()||a.pc!=0x126030))return false;
        return true;
    }
    bool withdrawSourceType0ArraysNative()noexcept {
        auto& a=sourceArrays_;if(!a.attempted||a.nativeWithdrawn)return true;
        if((sourceResource_.attempted&&!sourceResource_.nativeWithdrawn)||!preflightSourceType0ArraysNative())return false;
        a.vertices.raw.reset();a.normals.raw.reset();a.vertices.known.clear();a.normals.known.clear();
        a.nativeWithdrawn=true;return true; //No11B600/resource124/ref60/cache operation.
    }

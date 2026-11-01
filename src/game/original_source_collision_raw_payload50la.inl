//Private, model-independent 125890 leaf. No source +110 read occurs in this
//prefix. Only the enclosing exact callback frame can supply its actual file.
    struct RawPayload {
        struct Word {
            std::array<std::uint8_t,4> bytes{};
            unsigned known=0; //Native backing zero is not a source initialization.
            void store(std::uint32_t x)noexcept {
                for(unsigned i=0;i<4;++i)bytes[i]=std::uint8_t(x>>(8*i));known=15;
            }
            std::uint32_t bits()const noexcept {
                return std::uint32_t(bytes[0])|(std::uint32_t(bytes[1])<<8)|
                    (std::uint32_t(bytes[2])<<16)|(std::uint32_t(bytes[3])<<24);
            }
        };
        struct Read {
            File::ReadReceipt receipt{};Diagnostic diagnostic{};
            std::uint32_t callPC=0;std::size_t width=0;
            bool returned=false;Word after;
        };
        struct Array {
            std::unique_ptr<std::uint8_t[]> raw;
            std::vector<std::uint8_t> known;
            std::size_t bytes=0;std::uint32_t stride=0,count=0;
            bool metadata=false,attempted=false,published=false,zero=false;
            std::optional<const void*> payload;
            bool current()const noexcept {
                if(!attempted)return !raw&&!metadata&&!published&&!payload&&known.empty();
                if(!metadata||!published||!payload||bytes!=16u+std::size_t(stride)*count||
                   (stride!=6&&stride!=12)||known.size()!=bytes||
                   *payload!=(raw?static_cast<const void*>(raw.get()+16):nullptr))return false;
                for(auto k:known)if(k>1||(!raw&&k))return false;
                if(raw){for(unsigned i=0;i<4;++i)
                    if(!known[i]||!known[i+4]||raw[i]!=std::uint8_t(stride>>(8*i))||
                       raw[i+4]!=std::uint8_t(count>>(8*i)))return false;
                    for(unsigned i=8;i<16;++i)if(known[i])return false;
                }
                return true;
            }
        };
        //Raw malloc blocks: unlike vertex/normal arrays these have NO cookie
        //and NO constructor. Receipts retain every partial store and reused scratch lane.
        struct RecordPrefix {
            struct Block {
                std::unique_ptr<std::uint8_t[]> raw;
                std::vector<std::uint8_t> known;
                std::uint32_t count=0,stride=0;
                bool attempted=false;
                std::optional<const void*> payload;
                bool current(std::uint32_t expectedStride)const noexcept {
                    if(!attempted)return !raw&&known.empty()&&!payload&&!count&&!stride;
                    if(!payload||stride!=expectedStride||known.size()!=std::size_t(count)*stride||
                       *payload!=raw.get())return false;
                    for(auto k:known)if(k>1||(!raw&&k))return false;
                    return true;
                }
            } nodes,faces;
            std::optional<const void*> node114,face118;
            struct NodeReceipt { bool center=false,half=false,radius=false,complete=false; std::uint32_t radiusBits=0; };
            struct FaceReceipt { bool normalRead=false,normalStored=false,complete=false;
                unsigned converted=0;std::array<std::uint16_t,3> temporary{}; };
            bool entered=false,readFailed=false,nodesComplete=false,facesComplete=false,returned125890=false;
            std::optional<std::uint16_t> root10C; //SH only: upper half of source storage stays unknown.
            std::uint32_t pc=0,entryPC=0;
            std::array<Word,3> initialScratch,scratch;
            std::vector<Read> reads;
            std::vector<NodeReceipt> nr;
            std::vector<FaceReceipt> fr;
            bool allocate(Block& b,std::uint32_t count,std::uint32_t stride,std::uint32_t at)noexcept {
                pc=at;
                try{b.known.assign(std::size_t(count)*stride,0);}catch(...){b.known.clear();return false;}
                b.count=count;b.stride=stride;b.attempted=true;
                try{b.raw.reset(new std::uint8_t[std::size_t(count)*stride]{});}catch(...){b.raw.reset();}
                b.payload=b.raw.get();return true; //Zero count still calls malloc; NULL remains an outcome.
            }
            template<class ActualRead> bool readWord(Word& w,std::uint32_t at,ActualRead& read)noexcept {
                pc=at;reads.emplace_back();auto& r=reads.back();r.callPC=at;r.width=4;
                r.returned=read(w.bytes.data(),4,r.receipt,r.diagnostic);
                if(r.receipt.done<=4)w.known|=(1u<<r.receipt.done)-1u;r.after=w;
                if(!r.returned){readFailed=true;pc=r.diagnostic.sourcePC;return false;}return true;
            }
            template<class ActualRead> bool direct(Block& b,std::size_t offset,std::size_t width,std::uint32_t at,ActualRead& read)noexcept {
                pc=at;if(!b.raw)return false; //Bound before the actual NULL-destination IO call.
                reads.emplace_back();auto& r=reads.back();r.callPC=at;r.width=width;
                r.returned=read(b.raw.get()+offset,width,r.receipt,r.diagnostic);
                if(r.receipt.done<=width)for(std::size_t i=0;i<r.receipt.done;++i)b.known[offset+i]=1;
                for(unsigned i=0;i<width;++i){r.after.bytes[i]=b.raw[offset+i];r.after.known|=unsigned(b.known[offset+i])<<i;}
                if(!r.returned){readFailed=true;pc=r.diagnostic.sourcePC;return false;}return true;
            }
            void store(Block& b,std::size_t offset)noexcept {
                for(unsigned lane=0;lane<3;++lane)for(unsigned i=0;i<4;++i){
                    b.raw[offset+4*lane+i]=scratch[lane].bytes[i];
                    b.known[offset+4*lane+i]=std::uint8_t((scratch[lane].known>>i)&1u);
                }
            }
            static void storeWord(Block& b,std::size_t offset,std::uint32_t bits,unsigned width)noexcept {
                for(unsigned i=0;i<width;++i){b.raw[offset+i]=std::uint8_t(bits>>(8*i));b.known[offset+i]=1;}
            }
            template<class ActualRead> void begin(const Word& nodeCount,const Word& faceCount,
                const std::array<Word,3>& e8,std::uint32_t at,ActualRead& read)noexcept {
                entered=true;pc=entryPC=at;initialScratch=scratch=e8;
                if(nodeCount.known!=15||nodeCount.bits()>32767u)return; //Native cap, not source validation.
                try{nr.resize(nodeCount.bits());reads.reserve(std::size_t(nodeCount.bits())*9u);}catch(...){nr.clear();return;}
                if(!allocate(nodes,nodeCount.bits(),36,0x126038))return;node114=nodes.payload;
                for(std::uint32_t i=0;i<nodes.count;++i){
                    const auto off=std::size_t(i)*36u;auto& r=nr[i];
                    if(!direct(nodes,off,4,0x126074,read))return;
                    constexpr std::uint32_t cp[3]={0x126090,0x1260ac,0x1260c8};
                    for(unsigned lane=0;lane<3;++lane)if(!readWord(scratch[lane],cp[lane],read))return;
                    store(nodes,off+12);r.center=true;
                    constexpr std::uint32_t hp[3]={0x126128,0x126144,0x126160};
                    for(unsigned lane=0;lane<3;++lane)if(!readWord(scratch[lane],hp[lane],read))return;
                    store(nodes,off+24);r.half=true;pc=0x1261b0;
                    std::array<std::uint32_t,3> input{};
                    for(unsigned lane=0;lane<3;++lane){if(scratch[lane].known!=15)return;input[lane]=scratch[lane].bits();}
                    //Bounded separately rounded native binary32, NOT guest FCSR/ACC parity.
                    if(!SourceCollisionPayloadMath50LA::radius1261B0(input,r.radiusBits))return;
                    storeWord(nodes,off+4,r.radiusBits,4);r.radius=true;
                    if(!direct(nodes,off+8,2,0x126218,read)||!direct(nodes,off+10,2,0x126234,read))return;
                    r.complete=true;
                }
                nodesComplete=true;pc=0x126250;
                if(faceCount.known!=15||faceCount.bits()>32768u)return;
                try{fr.resize(faceCount.bits());reads.reserve(reads.size()+std::size_t(faceCount.bits())*7u);}catch(...){fr.clear();return;}
                if(!allocate(faces,faceCount.bits(),20,0x12625c))return;face118=faces.payload;
                for(std::uint32_t i=0;i<faces.count;++i){
                    const auto off=std::size_t(i)*20u;auto& r=fr[i];
                    if(!direct(faces,off+8,4,0x126298,read))return;
                    constexpr std::uint32_t fp[3]={0x1262b4,0x1262d0,0x1262ec};
                    for(unsigned lane=0;lane<3;++lane)if(!readWord(scratch[lane],fp[lane],read))return;
                    r.normalRead=true;
                    constexpr std::uint32_t cv[3]={0x126320,0x126348,0x126360};
                    for(unsigned lane=0;lane<3;++lane){pc=cv[lane];
                        if(scratch[lane].known!=15||!SourceCollisionPayloadMath50LA::faceLane126320(
                            scratch[lane].bits(),r.temporary[lane]))return;
                        r.converted|=1u<<lane;
                    }
                    //All three stackD0/D2/D4 conversions precede any destination normal store.
                    for(unsigned lane=0;lane<3;++lane)storeWord(faces,off+12+lane*2,r.temporary[lane],2);
                    r.normalStored=true;
                    constexpr std::uint32_t ix[3]={0x1263a0,0x1263bc,0x1263d8};
                    for(unsigned lane=0;lane<3;++lane)if(!direct(faces,off+lane*2,2,ix[lane],read))return;
                    r.complete=true;
                }
                facesComplete=true;root10C=0;returned125890=true;pc=0x126410;
                //Control return only: no fabricated V0, model, resource or outer load return.
            }
            bool current(const File::ReadReceipt& preceding,const Word& nc,const Word& fc,
                const std::array<Word,3>& e8)const noexcept {
                if(!entered)return !pc&&!entryPC&&reads.empty()&&!nodes.attempted&&!faces.attempted;
                if(!pc||(entryPC!=0x12602c&&entryPC!=0x126030&&entryPC!=0x125d9c)||
                   !nodes.current(36)||!faces.current(20)||node114!=nodes.payload||face118!=faces.payload)return false;
                for(unsigned lane=0;lane<3;++lane)if(initialScratch[lane].bytes!=e8[lane].bytes||
                    initialScratch[lane].known!=e8[lane].known)return false;
                if(nodes.attempted&&(nc.known!=15||nodes.count!=nc.bits()||nodes.count>32767u||nr.size()!=nodes.count))return false;
                if(faces.attempted&&(!nodesComplete||fc.known!=15||faces.count!=fc.bits()||faces.count>32768u||fr.size()!=faces.count))return false;
                const auto nodeReads=std::size_t(nodes.count)*9u;
                if(reads.size()>nodeReads+std::size_t(faces.count)*7u||(!reads.empty()&&!nodes.attempted))return false;
                constexpr std::uint32_t np[9]={0x126074,0x126090,0x1260ac,0x1260c8,0x126128,0x126144,0x126160,0x126218,0x126234};
                constexpr std::uint32_t fp[7]={0x126298,0x1262b4,0x1262d0,0x1262ec,0x1263a0,0x1263bc,0x1263d8};
                auto expectedScratch=initialScratch;const auto* prior=&preceding;unsigned failures=0;
                for(std::size_t i=0;i<reads.size();++i){const auto& r=reads[i];const auto& f=r.receipt;
                    const bool node=i<nodeReads;const auto step=node?i%9u:(i-nodeReads)%7u;
                    const auto width=node?(step<7?4u:2u):(step<4?4u:2u);
                    const bool scratchRead=step>0&&step<(node?7u:4u);
                    if(r.callPC!=(node?np[step]:fp[step])||r.width!=width||f.done>width)return false;
                    if(f.entered){if(f.requested!=width||(prior->afterKnown&&f.before!=prior->after)||
                        (f.afterKnown&&(f.after<f.before||f.after-f.before!=f.done)))return false;}
                    else if(f.returned||f.done||f.afterKnown)return false;
                    if(r.returned){if(!f.entered||!f.returned)return false;}
                    else{++failures;if(i+1!=reads.size()||f.returned||pc!=r.diagnostic.sourcePC||
                        (f.entered&&f.diagnostic.sourcePC!=pc))return false;}
                    Word before;if(scratchRead)before=expectedScratch[(step-1)%3];
                    if(r.after.known!=(before.known|((1u<<f.done)-1u)))return false;
                    if(scratchRead){for(std::size_t b=f.done;b<4;++b)if(r.after.bytes[b]!=before.bytes[b])return false;
                        expectedScratch[(step-1)%3]=r.after;}
                    prior=&f;
                }
                if(readFailed!=(failures==1)||failures>1)return false;
                for(unsigned lane=0;lane<3;++lane)if(scratch[lane].bytes!=expectedScratch[lane].bytes||
                    scratch[lane].known!=expectedScratch[lane].known)return false;
                const auto returned=[&](std::size_t i){return i<reads.size()&&reads[i].returned;};
                //Reconstruct each record in a fixed small native buffer. No allocation,
                //IO, floating-point reevaluation, or promotion of unknown padding here.
                const auto blockCurrent=[&](const Block& b,bool node)noexcept {
                    if(!b.attempted)return true;
                    for(std::uint32_t i=0;i<b.count;++i){
                        const auto first=node?std::size_t(i)*9u:nodeReads+std::size_t(i)*7u;
                        const auto off=std::size_t(i)*b.stride;
                        std::array<std::uint8_t,36> bytes{},known{};
                        const auto apply=[&](unsigned at,const Word& w,unsigned width){
                            for(unsigned j=0;j<width;++j){bytes[at+j]=w.bytes[j];known[at+j]=std::uint8_t((w.known>>j)&1u);}};
                        const auto import=[&](unsigned step,unsigned at,unsigned width){if(first+step<reads.size())apply(at,reads[first+step].after,width);};
                        import(0,node?0u:8u,4);
                        if(node){const auto& r=nr[i];
                            if(r.center!=returned(first+3)||r.half!=returned(first+6)||r.complete!=returned(first+8)||
                               (r.radius&&!r.half)||(first+7<reads.size()&&!r.radius))return false;
                            if(r.center)for(unsigned lane=0;lane<3;++lane)apply(12+lane*4,reads[first+1+lane].after,4);
                            if(r.half)for(unsigned lane=0;lane<3;++lane)apply(24+lane*4,reads[first+4+lane].after,4);
                            if(r.radius){for(unsigned lane=0;lane<3;++lane)if(reads[first+4+lane].after.known!=15)return false;
                                Word w;w.store(r.radiusBits);apply(4,w,4);}
                            import(7,8,2);import(8,10,2);
                        }else{const auto& r=fr[i];
                            if(r.normalRead!=returned(first+3)||r.complete!=returned(first+6)||
                               (r.converted!=0&&r.converted!=1&&r.converted!=3&&r.converted!=7)||
                               (r.converted&&!r.normalRead)||r.normalStored!=(r.converted==7)||
                               (first+4<reads.size()&&!r.normalStored))return false;
                            for(unsigned lane=0;lane<3;++lane){if((r.converted&(1u<<lane))&&reads[first+1+lane].after.known!=15)return false;
                                if(r.normalStored){Word w;w.store(r.temporary[lane]);apply(12+lane*2,w,2);}}
                            import(4,0,2);import(5,2,2);import(6,4,2);
                        }
                        for(unsigned j=0;j<b.stride;++j){if(b.known[off+j]!=known[j]||
                            (b.raw&&known[j]&&b.raw[off+j]!=bytes[j])||(!b.raw&&known[j]))return false;}
                    }return true;
                };
                if(!blockCurrent(nodes,true)||!blockCurrent(faces,false))return false;
                const bool allNodes=nodes.attempted&&(nodes.count==0||returned(nodeReads-1));
                const bool allFaces=faces.attempted&&(faces.count==0||returned(nodeReads+std::size_t(faces.count)*7u-1));
                if(nodesComplete!=allNodes||facesComplete!=allFaces||returned125890!=facesComplete||
                   root10C.has_value()!=facesComplete||(root10C&&*root10C!=0))return false;
                if(returned125890&&(pc!=0x126410||readFailed))return false;
                if(!readFailed&&!returned125890){
                    if(nodes.attempted&&!nodesComplete&&nodes.raw){
                        const auto i=reads.size()/9u;const auto& r=nr[i];
                        if(r.half&&pc!=0x1261b0)return false;
                    }
                    if(faces.attempted&&!facesComplete&&faces.raw){
                        const auto i=(reads.size()-nodeReads)/7u;const auto& r=fr[i];
                        if(r.normalRead&&pc!=(r.converted==0?0x126320u:r.converted==1?0x126348u:0x126360u))return false;
                    }
                }
                return true;
            }
        } records;
        bool entered=false,scalarComplete=false,arraysAdmitted=false;
        bool verticesComplete=false,presenceComplete=false,normalsComplete=false,normalNullStored=false;
        bool readFailed=false;
        std::uint32_t pc=0,count=0,verticesStored=0,normalsStored=0;
        std::array<Word,16> matrix40;
        Word reverseDC,vertex11C,nodesE0,facesE4,packedF4,packed108,presenceF8;
        std::array<Word,3> vertexScratch,normalScratch;
        const OriginalApplicationEntities50LA* application=nullptr; //Historical identity, never dereferenced by currentness.
        std::array<Word,3> guardScratch;
        std::array<std::uint64_t,3> widened{};
        std::array<std::uint32_t,3> classified{};
        std::array<OriginalCollisionNormalGuards50LA::Receipt,3> guards{};
        unsigned mathMask=0;
        bool floatGuardEntered=false,floatGuardsReturned=false;
        std::vector<Read> reads;
        Array vertices,normals;
        std::optional<const void*> vertex100,normal104; //Actual shape aliases, including storedNULL.
        template<class ActualRead>
        bool word(Word& out,std::size_t width,std::uint32_t at,ActualRead& read)noexcept {
            pc=at; //Receipt storage was admitted before any source call.
            reads.emplace_back();auto& r=reads.back();r.callPC=at;r.width=width;
            r.returned=read(out.bytes.data(),width,r.receipt,r.diagnostic);
            if(r.receipt.done<=width)out.known|=(1u<<r.receipt.done)-1u;
            r.after=out;
            if(!r.returned){readFailed=true;pc=r.diagnostic.sourcePC;return false;}
            return true; //Original ignores short read counts; knownness carries them.
        }
        bool allocate(Array& a,std::uint32_t stride,bool zero,std::uint32_t at)noexcept {
            pc=at;const auto bytes=16u+std::size_t(stride)*count;
            //Native knownness failure is not the original nullable allocation.
            try{a.known.assign(bytes,0);}catch(...){a.known.clear();return false;}
            a.bytes=bytes;a.stride=stride;a.count=count;a.zero=zero;a.metadata=true;a.attempted=true;
            try{a.raw.reset(new std::uint8_t[bytes]{});}catch(...){a.raw.reset();}
            if(a.raw){
                for(unsigned i=0;i<4;++i){a.raw[i]=std::uint8_t(stride>>(8*i));
                    a.raw[i+4]=std::uint8_t(count>>(8*i));a.known[i]=a.known[i+4]=1;}
                //2E89D0/D4 cookie. Padding8..15 unknown. 126430/11C090
                //zero constructors, versus packed-normal126420 with no stores.
                if(zero)for(std::size_t i=16;i<bytes;++i)a.known[i]=1;
            }
            a.payload=a.raw?static_cast<const void*>(a.raw.get()+16):nullptr;
            a.published=true;return true; //Nullable2E8990 publishes NULL too.
        }
        bool store(Array& a,std::uint32_t index,const std::array<Word,3>& scratch,
                   std::size_t width,std::uint32_t at)noexcept {
            pc=at;if(!a.raw)return false; //After the three reads, not allocation.
            const auto offset=16u+std::size_t(index)*a.stride;
            for(unsigned lane=0;lane<3;++lane)for(unsigned i=0;i<width;++i){
                a.raw[offset+lane*width+i]=scratch[lane].bytes[i];
                a.known[offset+lane*width+i]=std::uint8_t((scratch[lane].known>>i)&1u);
            }
            return true;
        }
        template<class ActualRead> void begin125890(OriginalApplicationEntities50LA& app,ActualRead&& read)noexcept {
            if(entered)return;entered=true;application=&app;pc=0x125890;
            try{reads.reserve(17);}catch(...){return;}
            constexpr std::uint32_t m[12]={0x1258c0,0x1258dc,0x1258f8,0x125918,0x125934,0x125950,
                0x125970,0x12598c,0x1259a8,0x1259c8,0x1259e4,0x125a00};
            for(unsigned row=0;row<4;++row){
                for(unsigned lane=0;lane<3;++lane)
                    if(!word(matrix40[4*row+lane],4,m[3*row+lane],read))return;
                matrix40[4*row+3].store(row==3?0x3f800000u:0u);
            }
            if(!word(reverseDC,4,0x125a24,read)||!word(vertex11C,4,0x125a40,read)||
               !word(nodesE0,4,0x125a5c,read)||!word(facesE4,4,0x125a78,read))return;
            packedF4.store(1);if(!word(packedF4,4,0x125a98,read))return;
            packed108=packedF4;scalarComplete=true;
            const bool packed=packedF4.bits()!=0;pc=packed?0x125ab4u:0x125ba0u;
            if(vertex11C.known!=15)return;
            count=vertex11C.bits();
            //Existing native65535 admission limit, NOT source count clamping.
            if(count>65535u)return;
            try{reads.reserve(18u+std::size_t(count)*6u);}catch(...){return;}
            arraysAdmitted=true;const std::size_t width=packed?2u:4u;
            if(!allocate(vertices,packed?6u:12u,true,packed?0x125ac4u:0x125bb0u))return;
            vertex100=vertices.payload;
            constexpr std::uint32_t vp[2][3]={{0x125bf4,0x125c10,0x125c2c},{0x125b14,0x125b30,0x125b4c}};
            for(std::uint32_t i=0;i<count;++i){
                if(packed)for(auto& w:vertexScratch){w.bytes[0]=w.bytes[1]=0;w.known=3;}
                for(unsigned lane=0;lane<3;++lane)if(!word(vertexScratch[lane],width,vp[packed][lane],read))return;
                if(!store(vertices,i,vertexScratch,width,packed?0x125b70u:0x125c68u))return;
                ++verticesStored;
            }
            verticesComplete=true;if(!word(presenceF8,4,0x125ca4,read))return;
            presenceComplete=true;pc=0x125cac;if(presenceF8.known!=15)return;
            if(!presenceF8.bits()){normal104=nullptr;normalNullStored=true;normalsComplete=true;pc=0x12602c;continueRecords(read);return;}
            if(!allocate(normals,packed?6u:12u,!packed,packed?0x125cd4u:0x125db0u))return;
            normal104=normals.payload;
            if(!packed){
                floatGuardEntered=true;guardScratch=vertexScratch;
                constexpr std::uint32_t conversions[3]={0x125dd8,0x125e5c,0x125ee0};
                for(unsigned lane=0;lane<3;++lane){
                    pc=conversions[lane];if(guardScratch[lane].known!=15)return;
                    //Exact integer/software-double leaves; no host FP classification,
                    //no Physics model, and no finite-only replacement of source NaN test.
                    widened[lane]=SourceCollisionMath50LA::widen311038(guardScratch[lane].bits());
                    classified[lane]=SourceCollisionMath50LA::classify3022D0(widened[lane]);
                    mathMask|=1u<<lane;
                    const auto g=OriginalCollisionNormalGuards50LA::reachAfterClassifier3022D0(
                        lane,app,classified[lane],guards[lane]);
                    pc=g.sourcePC;if(g.status!=OriginalCollisionNormalGuards50LA::Status::returned)return;
                }
                floatGuardsReturned=true;
            }
            constexpr std::uint32_t np[2][3]={{0x125f84,0x125fa0,0x125fbc},{0x125d18,0x125d34,0x125d50}};
            auto& scratch=packed?normalScratch:vertexScratch; //Actual E8/EC/F0 reuse.
            for(std::uint32_t i=0;i<count;++i){
                for(unsigned lane=0;lane<3;++lane)if(!word(scratch[lane],width,np[packed][lane],read))return;
                if(!store(normals,i,scratch,width,packed?0x125d74u:0x125ff8u))return;++normalsStored;
            }
            normalsComplete=true;pc=!packed?0x12602cu:(nodesE0.known==15?0x126030u:0x125d9cu);
            continueRecords(read);
        }
        template<class ActualRead> void continueRecords(ActualRead& read)noexcept {
            //E8/EC/F0 was used by floating vertices only. Packed vertices use80
            //and packed normals88: do not accidentally promote either to E8.
            const std::array<Word,3> e8=packedF4.bits()?std::array<Word,3>{}:vertexScratch;
            records.begin(nodesE0,facesE4,e8,pc,read);pc=records.pc;
        }
        bool current(const File::ReadReceipt& preceding)const noexcept {
            if(!entered)return !pc&&reads.empty()&&!scalarComplete&&!vertices.raw&&!normals.raw;
            if(vertex100!=vertices.payload||normal104!=(normalNullStored?std::optional<const void*>(nullptr):normals.payload))return false;
            if(!pc||!vertices.current()||!normals.current()||verticesStored>count||normalsStored>count)return false;
            constexpr std::uint32_t sp[17]={0x1258c0,0x1258dc,0x1258f8,0x125918,0x125934,0x125950,
                0x125970,0x12598c,0x1259a8,0x1259c8,0x1259e4,0x125a00,
                0x125a24,0x125a40,0x125a5c,0x125a78,0x125a98};
            constexpr std::uint32_t vp[2][3]={{0x125bf4,0x125c10,0x125c2c},{0x125b14,0x125b30,0x125b4c}};
            constexpr std::uint32_t np[2][3]={{0x125f84,0x125fa0,0x125fbc},{0x125d18,0x125d34,0x125d50}};
            if(reads.size()>17u&&(!arraysAdmitted||reads.size()>18u+6u*count))return false;
            const File::ReadReceipt* prior=&preceding;
            unsigned failed=0;
            for(std::size_t i=0;i<reads.size();++i){const auto& r=reads[i];const auto& f=r.receipt;
                if(!r.callPC||(r.width!=2&&r.width!=4)||f.done>r.width)return false;
                if(i<17){if(r.callPC!=sp[i]||r.width!=4)return false;}
                else{
                    const auto n=i-17;const bool packed=packedF4.bits()!=0;
                    if(n<3u*count){if(r.callPC!=vp[packed][n%3]||r.width!=(packed?2u:4u))return false;}
                    else if(n==3u*count){if(r.callPC!=0x125ca4||r.width!=4)return false;}
                    else if(!normals.published||(!packed&&!floatGuardsReturned)||r.width!=(packed?2u:4u)||r.callPC!=np[packed][(n-3u*count-1u)%3])return false;
                }
                if(f.entered){if(f.requested!=r.width||(prior->afterKnown&&f.before!=prior->after)||
                    (f.afterKnown&&(f.after<f.before||f.after-f.before!=f.done)))return false;
                }else if(f.returned||f.done||f.afterKnown)return false;
                if(r.returned){if(!f.entered||!f.returned)return false;}
                else{++failed;if(i+1!=reads.size()||f.returned||pc!=r.diagnostic.sourcePC||
                    (f.entered&&f.diagnostic.sourcePC!=pc))return false;}
                unsigned priorKnown=0;
                if(i==16)priorKnown=15; //Actual packed flag initializer.
                else if(i>=17&&i<17u+3u*count){
                    if(packedF4.bits())priorKnown=3; //Each packed vertex lane zeroed.
                    else if(i>=20)priorKnown=reads[i-3].after.known;
                }else if(i>=18u+3u*count){
                    if(i>=21u+3u*count)priorKnown=reads[i-3].after.known;
                    else if(!packedF4.bits())priorKnown=guardScratch[(i-18u-3u*count)%3].known;
                }
                if(r.after.known!=(priorKnown|((1u<<f.done)-1u)))return false;
                if(i>=18u+3u*count&&!packedF4.bits()){
                    const auto lane=(i-18u-3u*count)%3;
                    const auto& before=i>=21u+3u*count?reads[i-3].after:guardScratch[lane];
                    for(std::size_t b=f.done;b<4;++b)if(r.after.bytes[b]!=before.bytes[b])return false;
                }
                prior=&f;
            }
            if(readFailed!=(failed==1)||failed>1)return false;
            const auto mask=[&](std::size_t i)noexcept {return i<reads.size()?(1u<<reads[i].receipt.done)-1u:0u;};
            for(unsigned row=0;row<4;++row){
                for(unsigned lane=0;lane<3;++lane)if(matrix40[4*row+lane].known!=mask(3*row+lane))return false;
                const auto last=3u*row+2u;const bool stored=last<reads.size()&&reads[last].returned;
                if(matrix40[4*row+3].known!=(stored?15u:0u)||
                   (stored&&matrix40[4*row+3].bits()!=(row==3?0x3f800000u:0u)))return false;
            }
            if(reverseDC.known!=mask(12)||vertex11C.known!=mask(13)||nodesE0.known!=mask(14)||facesE4.known!=mask(15))return false;
            const bool packedInitialized=reads.size()>15&&reads[15].returned;
            if(packedF4.known!=(packedInitialized?15u:0u)||scalarComplete!=(reads.size()>16&&reads[16].returned))return false;
            if(scalarComplete&&(reads.size()<17||!reads[16].returned||packed108.known!=15||
                packed108.bytes!=packedF4.bytes))return false;
            if(arraysAdmitted&&(!scalarComplete||vertex11C.known!=15||count!=vertex11C.bits()||count>65535))return false;
            if(vertices.published&&!arraysAdmitted)return false;
            const auto elementsCurrent=[&](const Array& a,std::size_t first,std::uint32_t stored)noexcept {
                std::uint32_t complete=0;
                for(std::uint32_t n=0;n<count;++n){
                    const auto at=first+3u*n;
                    if(at+2>=reads.size()||!reads[at+2].returned)break;
                    ++complete;
                    if(a.raw)for(unsigned lane=0;lane<3;++lane){const auto& w=reads[at+lane].after;
                        for(unsigned b=0;b<a.stride/3;++b){const auto offset=16u+std::size_t(n)*a.stride+lane*(a.stride/3)+b;
                            if(a.raw[offset]!=w.bytes[b]||a.known[offset]!=((w.known>>b)&1u))return false;}
                    }
                }
                if(stored!=(a.raw?complete:0u))return false;
                //All untouched constructor bytes retain their original knownness.
                if(a.raw)for(std::size_t n=16u+std::size_t(stored)*a.stride;n<a.bytes;++n)
                    if(a.known[n]!=(a.zero?1:0)||(a.zero&&a.raw[n]))return false;
                return true;
            };
            if(arraysAdmitted&&(!elementsCurrent(vertices,17,verticesStored)||
                !elementsCurrent(normals,18u+3u*count,normalsStored)))return false;
            if(verticesComplete&&(!vertices.published||verticesStored!=count))return false;
            if(presenceComplete&&(!verticesComplete||reads.size()<18u+3u*count))return false;
            if(normals.published&&(!presenceComplete||presenceF8.known!=15||!presenceF8.bits()))return false;
            if(normalNullStored&&(!presenceComplete||presenceF8.known!=15||presenceF8.bits()||
                normals.published||!normalsComplete||(!records.entered&&pc!=0x12602c)))return false;
            if(normalsComplete&&!normalNullStored&&(!normals.published||normalsStored!=count||
                (!packedF4.bits()&&!floatGuardsReturned)||(!records.entered&&pc!=0x125d9c&&pc!=0x126030&&pc!=0x12602c)))return false;
            if(!packedF4.bits()){
                std::array<Word,3> expected{};
                for(std::size_t i=17;i<reads.size();++i){
                    if(i==17u+3u*count)continue; //Presence is independent F8.
                    const auto lane=i<17u+3u*count?(i-17u)%3u:(i-18u-3u*count)%3u;
                    const auto& r=reads[i];
                    for(std::size_t b=r.receipt.done;b<4;++b)
                        if(r.after.bytes[b]!=expected[lane].bytes[b])return false;
                    expected[lane]=r.after;
                }
                for(unsigned lane=0;lane<3;++lane)if(vertexScratch[lane].bytes!=expected[lane].bytes||
                    vertexScratch[lane].known!=expected[lane].known)return false;
            }
            if(!application||floatGuardEntered!=(normals.published&&!packedF4.bits()))return false;
            if(mathMask!=0&&mathMask!=1&&mathMask!=3&&mathMask!=7)return false;
            for(unsigned lane=0;lane<3;++lane){
                if(!OriginalCollisionNormalGuards50LA::historyCurrent(guards[lane],application))return false;
                Word expected;
                if(count&&17u+3u*count<=reads.size())expected=reads[17u+3u*(count-1)+lane].after;
                if(floatGuardEntered&&(guardScratch[lane].known!=expected.known||guardScratch[lane].bytes!=expected.bytes))return false;
                const bool computed=(mathMask&(1u<<lane))!=0;
                if(computed){
                    if(!floatGuardEntered||guardScratch[lane].known!=15||
                       (lane&&!guards[lane-1].returned)||!guards[lane].entered||
                       guards[lane].lane!=lane||guards[lane].classifier!=classified[lane]||
                       widened[lane]!=SourceCollisionMath50LA::widen311038(guardScratch[lane].bits())||
                       classified[lane]!=SourceCollisionMath50LA::classify3022D0(widened[lane]))return false;
                }else if(guards[lane].entered||widened[lane]||classified[lane])return false;
            }
            if(floatGuardsReturned!=(floatGuardEntered&&mathMask==7&&guards[2].returned))return false;
            if(floatGuardEntered&&!floatGuardsReturned){
                if(reads.size()!=18u+3u*count||normalsStored||normalsComplete||records.entered)return false;
                unsigned lane=0;while(lane<3&&guards[lane].returned)++lane;
                constexpr std::uint32_t cp[3]={0x125dd8,0x125e5c,0x125ee0};
                if(lane==3||pc!=(guards[lane].entered?guards[lane].pc:cp[lane])||
                   (!guards[lane].entered&&guardScratch[lane].known==15))return false;
            }
            //Guard histories are immutable receipts. Cleanup does not reset process guards.
            const std::array<Word,3> e8=packedF4.bits()?std::array<Word,3>{}:vertexScratch;
            if(!records.current(reads.empty()?preceding:reads.back().receipt,nodesE0,facesE4,e8)||
               records.entered!=normalsComplete||
               (records.entered&&(pc!=records.pc||records.entryPC!=((normalNullStored||!packedF4.bits())?0x12602cu:
                   (nodesE0.known==15?0x126030u:0x125d9cu)))))return false;
            return true;
        }
    };

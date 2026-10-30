//Actual296190 type0 vertex selection and295E00 point-record stores.
    using PointMath=rr::assets::OriginalSourceContactPointMath50LA;
    struct PointWord {
        std::array<std::uint8_t,4> bytes{};std::uint8_t known=0;
        void store(std::uint32_t v)noexcept {for(unsigned b=0;b<4;++b)bytes[b]=std::uint8_t(v>>(8u*b));known=15;}
        std::uint32_t bits()const noexcept {return std::uint32_t(bytes[0])|(std::uint32_t(bytes[1])<<8)|
            (std::uint32_t(bytes[2])<<16)|(std::uint32_t(bytes[3])<<24);}
    };
    struct PointIteration {
        bool entered=false,packedRead=false,pointReady=false,inputCopied=false,capacityChecked=false;
        bool full=false,duplicate=false,modelStored=false,appended=false,returned=false;
        std::uint32_t ordinal=0,countBefore=0,countAfter=0,scan=0,pc=0;
        PointWord packed;
        std::array<PointWord,4> point,temp,tempBefore;
        std::array<PointWord,3> operands;
        std::uint8_t operandMask=0,convertedMask=0;
        //Each axis uses two bits:0unreached,1false,2true,3unresolved.
        //Full scan is retained even after an earlier duplicate match.
        std::array<std::uint8_t,50> comparisons{};
        std::uint64_t modelCompared=0,modelMatched=0;
    };
    struct PointRecordHistory {
        const OriginalRacerSceneObject50LA* model=nullptr;
        std::array<std::uint8_t,96> bytes{},known{};
        std::uint8_t flagsKnown=0,stores=0;
    };
    struct SourcePoints {
        bool attempted=false,borrowed=false,metadataReady=false,returned=false,nativeWithdrawn=false;
        OriginalSourcePhysicsCollision50LA* collision=nullptr;
        const void* collisionHistory=nullptr;
        const OriginalRacerSceneObject50LA* model=nullptr;
        PointWord loopCount;
        std::array<PointWord,4> persistentTemp{}; //Actual stack70..7C survives loop iterations.
        std::uint32_t ordinal=0,recordCount=0,pc=0,countReads=0;
        std::vector<PointIteration> iterations;
        std::array<PointRecordHistory,50> records{};
    } sourcePoints_;
    static std::int32_t pointSigned(std::uint32_t bits)noexcept {std::int32_t v;std::memcpy(&v,&bits,4);return v;}
    bool readPointCount2962F0()noexcept {
        auto& p=sourcePoints_;p.pc=0x2962f0;
        if(!p.collision->readSourceContactCount2962F0(this,p.loopCount.bytes,p.loopCount.known))return false;
        ++p.countReads;return p.loopCount.known==15;
    }
    void copyPointRecord(unsigned record,unsigned offset,const PointWord& word)noexcept {
        auto& h=sourcePoints_.records[record];const auto base=96u*record+offset;
        for(unsigned b=0;b<4;++b){h.bytes[offset+b]=word.bytes[b];h.known[offset+b]=std::uint8_t((word.known>>b)&1u);
            points80_[base+b]=word.bytes[b];points80Known_[base+b]=h.known[offset+b];}
    }
    bool appendSourcePoint295E00(PointIteration& it)noexcept {
        auto& p=sourcePoints_;it.pc=p.pc=0x295e0c;it.inputCopied=true; //Raw quad copy, including unknown lanes.
        it.countBefore=p.recordCount;it.countAfter=p.recordCount;
        it.pc=p.pc=0x295e18;it.capacityChecked=true;
        if(pointSigned(p.recordCount)>=pointSigned(storage_->words[0x88/4])){
            it.full=true;it.returned=true;it.pc=p.pc=0x295fc4;return true;}
        for(std::uint32_t n=0;n<p.recordCount;++n){
            it.scan=n;
            for(unsigned axis=0;axis<3;++axis){
                static constexpr std::uint32_t loadPC[3]={0x295e50,0x295e70,0x295e90};
                static constexpr std::uint32_t comparePC[3]={0x295e54,0x295e74,0x295e94};
                it.pc=p.pc=loadPC[axis];if(!points80_)return false;
                const auto& old=p.records[n];const auto off=0x10u+4u*axis;
                std::uint32_t oldBits=0;bool oldKnown=true;
                for(unsigned b=0;b<4;++b){oldKnown=oldKnown&&old.known[off+b];oldBits|=std::uint32_t(old.bytes[off+b])<<(8u*b);}
                it.pc=p.pc=comparePC[axis];bool equal=false;
                if(!oldKnown||it.point[axis].known!=15||!PointMath::equal295E54(it.point[axis].bits(),oldBits,equal)){
                    it.comparisons[n]|=std::uint8_t(3u<<(2u*axis));return false;}
                it.comparisons[n]|=std::uint8_t((equal?2u:1u)<<(2u*axis));
                if(!equal)break;
                if(axis==2){
                    it.pc=p.pc=0x295eb4;it.modelCompared|=std::uint64_t{1}<<n;
                    if(old.model==p.model){it.modelMatched|=std::uint64_t{1}<<n;it.duplicate=true;}
                }
            }
            it.scan=n+1; //Original continues scanning after a duplicate match.
        }
        if(it.duplicate){it.returned=true;it.pc=p.pc=0x295fc4;return true;}
        const auto slot=p.recordCount;it.pc=p.pc=0x295efc;
        if(!points80_||slot>=50)return false; //Actual first Model40 store, not a NULL-safe skip.
        auto& h=p.records[slot];h.model=p.model;h.stores|=1;it.modelStored=true;
        //Pointer40 remains typed only; no serialized host/guest pointer bits.
        it.pc=p.pc=0x295f24;
        for(unsigned axis=0;axis<4;++axis)copyPointRecord(slot,0x10u+4u*axis,it.point[axis]);h.stores|=2;
        it.pc=p.pc=0x295f4c;
        //Only sourcebit0 becomes known. Never read uninitialized host upperbits.
        h.bytes[0x50]=1;h.flagsKnown=1;points80_[96u*slot+0x50]=1;h.stores|=4;
        it.pc=p.pc=0x295f68;PointWord zero;zero.store(0);copyPointRecord(slot,0x48,zero);h.stores|=8;
        it.pc=p.pc=0x295f88;
        for(unsigned axis=0;axis<4;++axis){PointWord w;w.store((*origin4435A0_)[axis]);copyPointRecord(slot,0x20+4*axis,w);}h.stores|=16;
        it.pc=p.pc=0x295fac;
        for(unsigned axis=0;axis<4;++axis){PointWord w;w.store((*origin4435A0_)[axis]);copyPointRecord(slot,0x30+4*axis,w);}h.stores|=32;
        it.pc=p.pc=0x295fb8;++p.recordCount;storage_->word(0x7c,p.recordCount);
        storage_->physical.spring.pointCount7C=p.recordCount;h.stores|=64;
        it.appended=true;it.countAfter=p.recordCount;it.returned=true;it.pc=p.pc=0x295fc4;return true;
    }
    Result selectSourcePoints296190(OriginalSourcePhysicsCollision50LA& col,std::uint32_t actualMode,const void* actualModelBASE)noexcept {
        auto& p=sourcePoints_;
        if(p.attempted||actualMode!=1||!bodyStored_||pc_!=0x1660d8||!preflightNative()||!bodyOwner_||
           col.base_!=base_||col.bodyBorrow_!=bodyOwner_||!col.attachReturned_||col.detachedNative_||
           !col.transformReturned_||!col.preflightNative())return {Status::owner,0x296190};
        const auto* model=bodyOwner_->sourceModel288Native();
        if(!model||actualModelBASE!=model||base_->object_->model14C!=actualModelBASE||!col.pinSourceContactPoints296190(this,model))return {Status::owner,0x296190};
        p.attempted=true;p.borrowed=true;p.collision=&col;p.collisionHistory=&col;p.model=model;p.pc=0x2961f0;
        if(!readPointCount2962F0())return at(Status::bounded,p.pc);
        const auto signedCount=pointSigned(p.loopCount.bits());
        if(signedCount<=0){p.returned=true;p.pc=0x2963d4;return at(Status::reached,p.pc);}
        //Existing65535 native allocation envelope, not an original count clamp.
        if(signedCount>65535)return at(Status::bounded,p.pc=0x2962f4);
        try{p.iterations.resize(static_cast<std::size_t>(signedCount));}catch(...){return at(Status::bounded,p.pc=0x2962f4);}
        p.metadataReady=true;
        while(pointSigned(p.ordinal)<pointSigned(p.loopCount.bits())){
            if(p.ordinal>=p.iterations.size())return at(Status::bounded,p.pc=0x2962f4);
            auto& it=p.iterations[p.ordinal];it.entered=true;it.ordinal=p.ordinal;it.tempBefore=p.persistentTemp;it.temp=p.persistentTemp;it.pc=p.pc=0x2961f8;
            for(unsigned lane=0;lane<4;++lane)it.point[lane].store(lane==3?0x3f800000u:0u);
            it.pc=p.pc=0x296200;
            if(!col.readSourceContactPacked296200(this,it.packed.bytes,it.packed.known)||it.packed.known!=15)
                return at(Status::bounded,p.pc);
            it.packedRead=true;const bool packed=it.packed.bits()!=0;
            for(unsigned axis=0;axis<3;++axis){
                static constexpr std::uint32_t packedRead[3]={0x296224,0x296254,0x296270};
                static constexpr std::uint32_t packedMath[3]={0x296244,0x296260,0x29627c};
                static constexpr std::uint32_t packedStore[3]={0x296250,0x29626c,0x296288};
                static constexpr std::uint32_t floatRead[3]={0x2962b4,0x2962bc,0x2962c4};
                static constexpr std::uint32_t floatStore[3]={0x2962b8,0x2962c0,0x2962c8};
                it.pc=p.pc=packed?packedRead[axis]:floatRead[axis];
                auto& word=it.operands[axis];
                if(!col.readSourceContactVertex296224(this,p.ordinal,axis,packed,word.bytes,word.known))return at(Status::bounded,p.pc);
                it.operandMask|=std::uint8_t(1u<<axis);
                if(packed){
                    it.pc=p.pc=packedMath[axis];std::uint32_t value=0;
                    if(word.known!=3||!PointMath::packed296244(static_cast<std::uint16_t>(word.bits()),value))return at(Status::bounded,p.pc);
                    it.pc=p.pc=packedStore[axis];it.temp[axis].store(value);p.persistentTemp[axis]=it.temp[axis];it.convertedMask|=std::uint8_t(1u<<axis);
                }else {it.pc=p.pc=floatStore[axis];it.point[axis]=word;}
            }
            if(packed){it.pc=p.pc=0x29628c;it.temp[3].store(0x3f800000u);p.persistentTemp[3]=it.temp[3];it.pc=p.pc=0x296298;it.point=it.temp;}
            else {it.pc=p.pc=0x2962cc;it.point[3].store(0x3f800000u);}
            it.pointReady=true;it.pc=p.pc=0x2962e0;
            if(!appendSourcePoint295E00(it))return at(Status::bounded,p.pc);
            p.pc=0x2962e8;++p.ordinal;
            if(!readPointCount2962F0())return at(Status::bounded,p.pc);
        }
        p.returned=true;p.pc=0x2963d4;return at(Status::reached,p.pc); //Control only; no numeric V0 invention.
    }
    bool preflightSourcePointsNative()const noexcept {
        const auto& p=sourcePoints_;
        if(!p.attempted){
            if(p.borrowed||p.metadataReady||p.returned||p.nativeWithdrawn||p.collision||p.collisionHistory||p.model||
               p.ordinal||p.recordCount||p.pc||p.countReads||!p.iterations.empty())return false;
            for(auto known:points80Known_)if(known)return false;
            return storage_&&storage_->known[0x7c/4]==0xffffffffu&&!storage_->words[0x7c/4];
        }
        if(!p.collisionHistory||!p.model||p.recordCount>50||p.borrowed==p.nativeWithdrawn||
           (p.nativeWithdrawn?bool(p.collision):static_cast<const void*>(p.collision)!=p.collisionHistory)||
           !bodyStored_||!bodyOwner_||!storage_||pc_!=p.pc)return false;
        if(!p.nativeWithdrawn&&(!p.collision->sourceContactPointCustodyNative(this)||
           bodyOwner_->sourceModel288Native()!=p.model))return false;
        if(storage_->known[0x7c/4]!=0xffffffffu||storage_->words[0x7c/4]!=p.recordCount||
           storage_->physical.spring.pointCount7C!=p.recordCount)return false;
        if(p.metadataReady){
            if(p.loopCount.known!=15||pointSigned(p.loopCount.bits())<=0||p.loopCount.bits()>65535||
               p.iterations.size()!=p.loopCount.bits()||p.ordinal>p.iterations.size())return false;
        }else if(!p.iterations.empty()||p.ordinal||p.recordCount)return false;
        std::uint32_t completed=0;
        std::array<PointWord,4> tempHistory{};
        for(std::size_t index=0;index<p.iterations.size();++index){const auto& it=p.iterations[index];
            if(!it.entered){if(index<p.ordinal||it.returned||it.pointReady||it.inputCopied||it.appended||it.duplicate)return false;continue;}
            if(index>p.ordinal||it.ordinal!=index||it.packed.known>15||it.operandMask>7||it.convertedMask>7||
               (index<p.ordinal&&!it.returned)||(index==p.ordinal&&it.returned))return false;
            for(const auto& w:it.point)if(w.known>15)return false;
            for(const auto& w:it.temp)if(w.known>15)return false;
            for(const auto& w:it.operands)if(w.known>15)return false;
            for(unsigned lane=0;lane<4;++lane){
                if(it.tempBefore[lane].bytes!=tempHistory[lane].bytes||it.tempBefore[lane].known!=tempHistory[lane].known)return false;
                const bool stored=lane<3?bool(it.convertedMask&(1u<<lane)):
                    (it.pointReady&&it.packedRead&&it.packed.bits()!=0);
                if(stored){if(it.temp[lane].known!=15)return false;
                    if(lane==3&&it.temp[lane].bits()!=0x3f800000u)return false;
                    tempHistory[lane]=it.temp[lane];
                }else if(it.temp[lane].bytes!=tempHistory[lane].bytes||it.temp[lane].known!=tempHistory[lane].known)return false;
            }
            if(it.packedRead&&it.packed.known!=15)return false;
            if(it.pointReady){
                if(!it.packedRead||it.operandMask!=7||it.point[3].known!=15||it.point[3].bits()!=0x3f800000u)return false;
                for(unsigned axis=0;axis<3;++axis){
                    if(it.packed.bits()){
                        if(it.operands[axis].known!=3||it.convertedMask!=7||it.temp[axis].known!=15||
                           it.point[axis].bytes!=it.temp[axis].bytes||it.point[axis].known!=15)return false;
                    }else if(it.convertedMask||it.point[axis].bytes!=it.operands[axis].bytes||
                             it.point[axis].known!=it.operands[axis].known)return false;
                }
            }
            if(it.inputCopied){
                if(!it.pointReady||!it.capacityChecked||it.countBefore!=completed||it.countAfter>50||
                   it.scan>it.countBefore||it.duplicate!=(it.modelMatched!=0)||it.modelMatched!=it.modelCompared)return false;
                std::uint64_t compared=0;
                for(unsigned old=0;old<50;++old){const auto code=it.comparisons[old];
                    if(code&0xc0u)return false;
                    if(old>=it.countBefore||it.full){if(code)return false;continue;}
                    bool stopped=false;unsigned tested=0;
                    for(unsigned axis=0;axis<3;++axis){const auto c=(code>>(2*axis))&3u;
                        if(stopped&&c)return false;
                        if(!c){stopped=true;continue;}++tested;
                        if(c!=2)stopped=true;
                        if(c==3&&(it.returned||old!=it.scan))return false;
                    }
                    if(code==0x2au)compared|=std::uint64_t{1}<<old;
                    if(old<it.scan&&!tested)return false;
                    if(old>it.scan&&code)return false;
                }
                if(compared!=it.modelCompared)return false;
                if(it.full){if(it.countBefore!=50||it.scan||it.duplicate||it.appended||it.modelStored||!it.returned)return false;}
                else if(it.returned&&it.scan!=it.countBefore)return false;
                if(it.appended){
                    if(it.full||it.duplicate||!it.returned||!it.modelStored||it.countBefore>=50||it.countAfter!=completed+1)return false;
                    const auto& h=p.records[completed];
                    for(unsigned axis=0;axis<4;++axis)for(unsigned byte=0;byte<4;++byte){const auto off=0x10+4*axis+byte;
                        if(h.bytes[off]!=it.point[axis].bytes[byte]||h.known[off]!=((it.point[axis].known>>byte)&1u))return false;}
                    ++completed;
                }else if(it.countAfter!=completed||it.modelStored)return false;
                if(it.returned&&it.pc!=0x295fc4)return false;
            }else if(it.capacityChecked||it.full||it.duplicate||it.modelStored||it.appended||it.returned||it.modelCompared||it.modelMatched)return false;
        }
        for(unsigned lane=0;lane<4;++lane)if(p.persistentTemp[lane].bytes!=tempHistory[lane].bytes||
            p.persistentTemp[lane].known!=tempHistory[lane].known)return false;
        if(completed!=p.recordCount)return false;
        for(unsigned slot=0;slot<50;++slot){const auto& h=p.records[slot];
            if(slot<p.recordCount){
                if(!points80_||h.model!=p.model||h.stores!=127||h.flagsKnown!=1||h.bytes[0x50]!=1||
                   (points80_[96u*slot+0x50]&1u)!=1u)return false;
                for(unsigned b=0;b<4;++b)if(!h.known[0x48+b]||h.bytes[0x48+b])return false;
                for(unsigned b=0;b<32;++b)if(!h.known[0x20+b])return false;
            }else if(h.model||h.stores||h.flagsKnown)return false;
            for(unsigned b=0;b<96;++b){const auto k=h.known[b];if(k>1||points80Known_[96u*slot+b]!=k)return false;
                if(k&&(!points80_||points80_[96u*slot+b]!=h.bytes[b]))return false;
                //Only these source stores can establish full-byte knowledge.
                if(k&&!((b>=0x10&&b<0x40)||(b>=0x48&&b<0x4c)))return false;
            }
        }
        if(p.countReads>p.ordinal+1)return false;
        if(p.returned)return p.loopCount.known==15&&pointSigned(p.ordinal)>=pointSigned(p.loopCount.bits())&&
            p.pc==0x2963d4&&p.countReads==p.ordinal+1;
        if(p.metadataReady&&p.ordinal<p.iterations.size()&&p.iterations[p.ordinal].entered)
            return p.pc==p.iterations[p.ordinal].pc;
        return p.pc==0x2962f0||p.pc==0x2962f4; //Initial/next signed-count or native metadata boundary.
    }
    bool withdrawSourcePointsNative()noexcept {
        if(detectorBorrower_)return false;
        auto& p=sourcePoints_;if(!p.attempted||p.nativeWithdrawn)return true;
        if(!preflightSourcePointsNative()||!p.collision->unpinSourceContactPointsNative(this))return false;
        p.collision=nullptr;p.borrowed=false;p.nativeWithdrawn=true;
        return true; //No source pointcount reset/Model retain-release/record rollback.
    }

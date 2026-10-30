//Included privately in the existing type0 shape/resource owner. This source
//prefix never invokes the prepared-mesh loader or constructs resource125014.
    enum class SourcePrefixStatus {bounded,owner,reached};
    struct SourcePrefixResult {SourcePrefixStatus status;std::uint32_t sourcePC;};
    struct SourcePrefixWord {
        std::array<std::uint8_t,4> bytes{}; //Backing zeros are not source knowledge.
        std::uint8_t known=0;
        void store(std::uint32_t value)noexcept {
            for(unsigned i=0;i<4;++i)bytes[i]=std::uint8_t(value>>(8u*i));known=15;
        }
        std::uint32_t bits()const noexcept {
            return std::uint32_t(bytes[0])|(std::uint32_t(bytes[1])<<8)|
                (std::uint32_t(bytes[2])<<16)|(std::uint32_t(bytes[3])<<24);
        }
    };
    struct SourcePrefixRead {
        bool entered=false,returned=false;
        std::size_t done=0;
        std::uint32_t pc=0;
    };
    struct SourcePrefix {
        bool attempted=false,cleanupReturned=false,discriminatorReturned=false;
        bool allocationAttempted=false,published=false,modelStored=false;
        bool scalarEntered=false,packedInitialized=false,packedStored=false,scalarReturned=false;
        bool nativeWithdrawn=false;
        std::shared_ptr<rr::assets::OriginalSourceCollisionFile50LA> file;
        const OriginalRacerSceneObject50LA* model=nullptr;
        const OriginalWreckComponent50LA* entity=nullptr;
        std::uint32_t* actual88=nullptr;
        std::optional<const void*>* actual8C=nullptr;
        std::optional<const void*>* actual90=nullptr;
        const Shape* publishedIdentity=nullptr;
        SourcePrefixWord discriminator;
        std::array<SourcePrefixWord,16> local40;
        SourcePrefixWord reverseDC,vertex11C,nodesE0,facesE4,packedF4,packed108;
        std::array<SourcePrefixRead,18> reads{};
        std::uint8_t readsEntered=0,readsReturned=0;
        std::uint32_t pc=0;
    } sourcePrefix_;
    template<class ActualRead>
    bool readSourcePrefixWord(SourcePrefixWord& destination,std::uint32_t pc,ActualRead& read)noexcept {
        auto& s=sourcePrefix_;
        if(s.readsEntered>=s.reads.size())return false;
        auto& receipt=s.reads[s.readsEntered++];receipt.entered=true;receipt.pc=pc;s.pc=pc;
        std::size_t copied=0;
        const bool returned=read(destination.bytes.data(),std::size_t{4},copied);
        receipt.done=copied;receipt.returned=returned;
        //The concrete COL file adapter bounds actual copied bytes by request.
        //Retain an impossible transport result as a refusal, never shift by it.
        if(copied>4)return false;
        destination.known=std::uint8_t(destination.known|((1u<<copied)-1u));
        if(returned)++s.readsReturned;
        return returned; //Short returned reads are NOT a source failure branch.
    }
    template<class ActualRead>
    SourcePrefixResult beginSourceType0Prefix124F80(
        const std::shared_ptr<rr::assets::OriginalSourceCollisionFile50LA>& actualFile,
        const OriginalRacerSceneObject50LA& actualModel,const OriginalWreckComponent50LA& actualEntity,
        std::uint32_t& actual88,std::optional<const void*>& actual8C,
        std::optional<const void*>& actual90,ActualRead&& read)noexcept {
        auto& s=sourcePrefix_;
        if(attempted_||retired_||s.attempted||!actualFile||!actual8C||*actual8C||!actual90||*actual90||
           &actual8C==&actual90||actual88||actualEntity.kind!=OriginalWreckComponentKind50LA::collision11B190||
           actualEntity.reference00!=1u||actualEntity.parent1C||actualEntity.previous10||actualEntity.next14||
           actualEntity.firstChild18||actualEntity.mask20||actualEntity.descendants24)
            return {SourcePrefixStatus::owner,0x124f80};
        //Only the concrete source COL owner can call this private method. It
        //authenticates live same-file/current handle and Body→Model custody,
        //then supplies the genuine size4,count1 read12FB30 callback.
        attempted_=true;s.attempted=true;s.file=actualFile;s.model=&actualModel;s.entity=&actualEntity;
        s.actual88=&actual88;s.actual8C=&actual8C;s.actual90=&actual90;s.pc=0x11b600;
        //Exact fresh NULL branch skips both frees but still reaches these stores.
        actual8C=nullptr;actual90=nullptr;s.cleanupReturned=true;
        s.discriminator.store(actual88); //Direct read preserves untouched constructor-known bytes.
        const bool discriminatorRead=readSourcePrefixWord(s.discriminator,0x124fb8,read);
        actual88=s.discriminator.bits();
        if(!discriminatorRead)return {SourcePrefixStatus::bounded,s.pc};
        s.discriminatorReturned=true;
        if(actual88!=0){s.pc=actual88==1?0x1250f8u:actual88==2?0x1253dcu:0x125414u;
            return {SourcePrefixStatus::bounded,s.pc};}
        s.allocationAttempted=true;s.pc=0x124ff0;
        try{shape8C_=std::make_unique<Shape>();}catch(...){shape8C_.reset();}
        actual8C=shape8C_.get();s.publishedIdentity=shape8C_.get();s.published=true;s.pc=0x125004;
        if(!shape8C_)return {SourcePrefixStatus::bounded,s.pc}; //Real nullable store then first dereference.
        shape8C_->racerModel110=&actualModel;s.modelStored=true;s.scalarEntered=true;
        constexpr std::array<std::uint32_t,12> matrixPC{{0x1258c0,0x1258dc,0x1258f8,
            0x125918,0x125934,0x125950,0x125970,0x12598c,0x1259a8,0x1259c8,0x1259e4,0x125a00}};
        for(unsigned row=0;row<4;++row){
            for(unsigned lane=0;lane<3;++lane)
                if(!readSourcePrefixWord(s.local40[4*row+lane],matrixPC[3*row+lane],read))
                    return {SourcePrefixStatus::bounded,s.pc};
            s.local40[4*row+3].store(row==3?0x3f800000u:0u); //Interleaved actual W stores.
        }
        if(!readSourcePrefixWord(s.reverseDC,0x125a24,read)||
           !readSourcePrefixWord(s.vertex11C,0x125a40,read)||
           !readSourcePrefixWord(s.nodesE0,0x125a5c,read)||
           !readSourcePrefixWord(s.facesE4,0x125a78,read))return {SourcePrefixStatus::bounded,s.pc};
        s.packedF4.store(1u);s.packedInitialized=true; //Actual125A94, unlike other unknown scratch.
        if(!readSourcePrefixWord(s.packedF4,0x125a98,read))return {SourcePrefixStatus::bounded,s.pc};
        s.packed108=s.packedF4;s.packedStored=true;s.scalarReturned=true;
        s.pc=s.packedF4.bits()?0x125ab4u:0x125ba0u;
        //First vertex-array allocation/cookie is an unjoined dependency. No
        //125890 return,125014 resource/ref60, cache or11B920 return is asserted.
        return {SourcePrefixStatus::bounded,s.pc};
    }
    bool preflightSourceType0PrefixNative()const noexcept {
        const auto& s=sourcePrefix_;
        if(!preflightSourceContactLeaseNative())return false;
        if(!s.attempted)return !s.nativeWithdrawn&&!s.file&&!s.model&&!s.entity;
        if(s.nativeWithdrawn)return retired_&&!shape8C_&&!loading_&&!s.file&&!s.actual88&&!s.actual8C&&!s.actual90&&
            (!sourceArrays_.attempted||sourceArrays_.nativeWithdrawn)&&
            (!sourceNodes_.attempted||sourceNodes_.nativeWithdrawn)&&
            (!sourceResource_.attempted||sourceResource_.nativeWithdrawn)&&preflightSourceType0ResourceNative()&&preflightSourceResultShapeNative()&&preflightSourceType0TransformNative();
        if(!preflightSourceType0ArraysNative()||!preflightSourceType0NodesNative()||!preflightSourceType0ResourceNative()||!preflightSourceResultShapeNative()||!preflightSourceType0TransformNative())return false;
        if(!attempted_||loaded_||retired_||freshWreck_||loading_||!s.file||!s.model||!s.entity||
           !s.actual88||!s.actual8C||!s.actual90||!s.cleanupReturned||!*s.actual8C||!*s.actual90||
           (!sourceResult_.attempted&&**s.actual90)||**s.actual8C!=shape8C_.get()||s.discriminator.known!=15||
           *s.actual88!=s.discriminator.bits()||s.readsEntered>18||s.readsReturned>s.readsEntered||
           s.entity->reference00!=1u||s.entity->kind!=OriginalWreckComponentKind50LA::collision11B190)return false;
        if(s.published!=s.allocationAttempted||s.publishedIdentity!=shape8C_.get()||
           (s.modelStored!=bool(shape8C_))||s.scalarEntered!=s.modelStored)return false;
        if(shape8C_&&(shape8C_->racerModel110!=s.model||shape8C_->model110.value||(!sourceResource_.attempted&&shape8C_->resource124)))return false;
        if(s.allocationAttempted&&(!s.discriminatorReturned||*s.actual88))return false;
        unsigned returned=0;
        for(unsigned i=0;i<s.reads.size();++i){const auto& r=s.reads[i];
            if(i<s.readsEntered){if(!r.entered||r.done>4||!r.pc)return false;if(r.returned)++returned;
                else if(i+1!=s.readsEntered)return false;}
            else if(r.entered||r.returned||r.done||r.pc)return false;
        }
        if(returned!=s.readsReturned||s.discriminatorReturned!=s.reads[0].returned)return false;
        constexpr std::array<std::uint32_t,18> pcs{{0x124fb8,0x1258c0,0x1258dc,0x1258f8,
            0x125918,0x125934,0x125950,0x125970,0x12598c,0x1259a8,0x1259c8,0x1259e4,0x125a00,
            0x125a24,0x125a40,0x125a5c,0x125a78,0x125a98}};
        for(unsigned i=0;i<s.readsEntered;++i)if(s.reads[i].pc!=pcs[i])return false;
        const auto readMask=[&](unsigned index)noexcept->std::uint8_t {
            return index<s.readsEntered?std::uint8_t((1u<<s.reads[index].done)-1u):0u;
        };
        for(unsigned row=0;row<4;++row){
            for(unsigned lane=0;lane<3;++lane){
                const unsigned index=4*row+lane;
                const auto originalKnown=index==5&&sourceResult_.store54Returned?
                    sourceResource_.matrix40[5].known:s.local40[index].known;
                if(originalKnown!=readMask(1+3*row+lane))return false;
            }
            const bool wStored=s.reads[3+3*row].returned;
            const auto& w=s.local40[4*row+3];
            if(w.known!=(wStored?15:0)||(wStored&&w.bits()!=(row==3?0x3f800000u:0u)))return false;
        }
        if(s.reverseDC.known!=readMask(13)||s.vertex11C.known!=readMask(14)||
           s.nodesE0.known!=readMask(15)||s.facesE4.known!=readMask(16)||
           s.packedInitialized!=s.reads[16].returned||s.packedF4.known!=(s.packedInitialized?15:0))return false;
        if(!s.scalarReturned){
            if(s.readsEntered&&!s.reads[s.readsEntered-1].returned){
                if(s.pc!=s.reads[s.readsEntered-1].pc)return false;
            }else if(!s.allocationAttempted){
                if(!s.discriminatorReturned||!*s.actual88||s.pc!=(*s.actual88==1?0x1250f8u:*s.actual88==2?0x1253dcu:0x125414u))return false;
            }else if(shape8C_||s.pc!=0x125004u)return false;
        }
        if(s.packedStored&&(!s.packedInitialized||s.packedF4.known!=15||
           s.packed108.known!=15||s.packed108.bytes!=s.packedF4.bytes))return false;
        if(s.scalarReturned)return s.packedStored&&s.readsEntered==18&&s.readsReturned==18&&
            s.pc==(s.packedF4.bits()?0x125ab4u:0x125ba0u);
        return !s.packedStored;
    }
    bool withdrawSourceType0PrefixNative()noexcept {
        auto& s=sourcePrefix_;
        if(!s.attempted||s.nativeWithdrawn)return true;
        if(sourceContactBorrower_||sourceResult_.live||sourceDirectTransform_.live||!preflightSourceType0PrefixNative()||!withdrawSourceType0TransformNative()||!withdrawSourceType0ResourceNative()||!withdrawSourceType0NodesNative()||!withdrawSourceType0ArraysNative())return false;
        //Native source-resource compensation (if reached) precedes arrays and
        //shape. This is not original11B600/destructor admission or cacheclear.
        *s.actual8C=nullptr;shape8C_.reset();retired_=true;s.nativeWithdrawn=true;
        s.file.reset();s.actual88=nullptr;s.actual8C=nullptr;s.actual90=nullptr;
        return true;
    }

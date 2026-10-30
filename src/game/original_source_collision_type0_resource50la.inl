//Private raw resource125014 suffix. Semantic/prepared Resource readiness is
//deliberately unchanged; source pointer words are typed aliases, not host bits.
    struct SourceResource {
        bool attempted=false,allocationAttempted=false,published=false,storesReturned=false,nativeWithdrawn=false;
        std::uint32_t pc=0;
        std::array<SourcePrefixWord,16> matrix40; //Immutable actual1250B0/B4 input snapshot.
        std::shared_ptr<Resource> pending;
        const Resource* publishedIdentity=nullptr;
    } sourceResource_;
    static void storeSourceResourceWord(Resource& resource,std::size_t offset,
        const SourcePrefixWord& word,std::size_t width=4)noexcept {
        auto& f=resource.source_;
        for(std::size_t i=0;i<width;++i){
            f.bytes[offset+i]=word.bytes[i];(*f.raw)[offset+i]=word.bytes[i];
            f.known[offset+i]=std::uint8_t((word.known>>i)&1u);
        }
    }
    SourcePrefixResult continueSourceType0Resource125014()noexcept {
        auto& r=sourceResource_;const auto& s=sourcePrefix_;const auto& n=sourceNodes_;const auto& a=sourceArrays_;
        if(r.attempted||!n.returned125890||n.pc!=0x126410||n.nativeWithdrawn||a.nativeWithdrawn||
           !shape8C_||shape8C_->resource124||!preflightSourceType0PrefixNative())
            return {SourcePrefixStatus::owner,n.pc};
        r.attempted=true;r.pc=0x125014;
        //Native control custody precedes the original nullable allocation. A
        //control failure is NOT a fabricated original malloc(NULL) return.
        try{r.pending=std::make_shared<Resource>();}catch(...){return {SourcePrefixStatus::bounded,r.pc};}
        auto& resource=*r.pending;auto& f=resource.source_;
        f.attempted=true;f.owner=this;f.file=s.file.get();r.allocationAttempted=true;
        try{f.raw=std::make_unique<std::array<std::uint8_t,0x70>>();}catch(...){f.raw.reset();}
        //Actual125024 precedes every resource store; NULL is published and
        //bounds at its first original dereference125030, retaining control.
        shape8C_->resource124=f.raw?r.pending:std::shared_ptr<Resource>{};
        r.publishedIdentity=shape8C_->resource124.get();r.published=true;r.pc=0x125030;
        if(!f.raw)return {SourcePrefixStatus::bounded,r.pc};
        SourcePrefixWord zero;zero.store(0);storeSourceResourceWord(resource,0,zero);f.storeMask|=1u;
        r.pc=0x125040;f.nodes08=n.nodes.payload;f.storeMask|=1u<<1;
        r.pc=0x125050;f.faces0C=n.faces.payload;f.storeMask|=1u<<2;
        r.pc=0x125060;storeSourceResourceWord(resource,4,s.vertex11C);f.storeMask|=1u<<3;
        r.pc=0x125070;f.vertices10=a.vertices.payload;f.storeMask|=1u<<4;
        r.pc=0x125078;f.normals14=a.normalZeroStored?std::optional<const void*>{nullptr}:a.normals.payload;f.storeMask|=1u<<5;
        r.pc=0x125080;storeSourceResourceWord(resource,0x18,s.packed108);f.storeMask|=1u<<6;
        r.pc=0x125090;SourcePrefixWord root;root.store(*n.root10C);storeSourceResourceWord(resource,0x1c,root,2);f.storeMask|=1u<<7;
        //Eight original pairs of word stores, not a transformed/decoded matrix.
        r.matrix40=s.local40;
        for(unsigned pair=0;pair<8;++pair){
            r.pc=0x1250b0;storeSourceResourceWord(resource,0x20+8*pair,s.local40[2*pair]);f.storeMask|=1u<<(8+2*pair);
            r.pc=0x1250b4;storeSourceResourceWord(resource,0x24+8*pair,s.local40[2*pair+1]);f.storeMask|=1u<<(9+2*pair);
        }
        r.pc=0x1250d4;SourcePrefixWord one;one.store(1);storeSourceResourceWord(resource,0x60,one);
        resource.reference60_=1;f.referenceBits60=1;f.refInitialized=true;f.storeMask|=1u<<24;
        r.storesReturned=true;r.pc=0x1250d8;
        return {SourcePrefixStatus::reached,r.pc}; //No1326A0 or124F80 return yet.
    }
    bool preflightSourceType0ResourceNative()const noexcept {
        const auto& r=sourceResource_;const auto& s=sourcePrefix_;
        if(!r.attempted)return !r.allocationAttempted&&!r.published&&!r.storesReturned&&!r.nativeWithdrawn&&!r.pending&&!r.publishedIdentity;
        if(r.nativeWithdrawn)return (!shape8C_||!shape8C_->resource124)&&
            (!r.pending||(r.pending->source_.nativeWithdrawn&&!r.pending->source_.raw&&
             !r.pending->reference60_&&!r.pending->sourceAssociationBorrower_&&!r.pending->sourceResultBorrower_&&!r.pending->sourceDirectTransformBorrower_));
        if(!s.attempted||s.nativeWithdrawn||!shape8C_||!s.file||!sourceNodes_.returned125890||
           sourceNodes_.nativeWithdrawn||sourceArrays_.nativeWithdrawn)return false;
        if(!r.pending)return !r.allocationAttempted&&!r.published&&!r.publishedIdentity&&!r.storesReturned&&
            !shape8C_->resource124&&r.pc==0x125014;
        const auto& resource=*r.pending;const auto& f=resource.source_;
        if(!f.attempted||f.nativeWithdrawn||f.owner!=this||f.file!=s.file.get()||
           resource.constructed_||resource.retired_||resource.borrowedBacking_||
           !r.allocationAttempted||!r.published||r.publishedIdentity!=shape8C_->resource124.get())return false;
        if(!f.raw)return !shape8C_->resource124&&!r.publishedIdentity&&!r.storesReturned&&
            !f.storeMask&&!f.refInitialized&&!resource.reference60_&&!resource.sourceAssociationBorrower_&&r.pc==0x125030;
        if(shape8C_->resource124!=r.pending||!r.storesReturned||r.pc!=0x1250d8||
           !resource.sourceAssociationReady1326A0(s.file.get(),this))return false;
        if(f.nodes08!=sourceNodes_.nodes.payload||f.faces0C!=sourceNodes_.faces.payload||
           f.vertices10!=sourceArrays_.vertices.payload||f.normals14!=(sourceArrays_.normalZeroStored?
           std::optional<const void*>{nullptr}:sourceArrays_.normals.payload))return false;
        std::array<std::uint8_t,0x70> expected{},known{};
        const auto copy=[&](std::size_t offset,const SourcePrefixWord& word,std::size_t width=4){
            for(std::size_t i=0;i<width;++i){expected[offset+i]=word.bytes[i];known[offset+i]=std::uint8_t((word.known>>i)&1u);}};
        SourcePrefixWord zero;zero.store(0);copy(0,zero);copy(4,s.vertex11C);copy(0x18,s.packed108);
        SourcePrefixWord root;root.store(*sourceNodes_.root10C);copy(0x1c,root,2);
        for(unsigned lane=0;lane<16;++lane){
            const auto& snapshot=r.matrix40[lane];const auto& current=s.local40[lane];
            if(lane==5&&sourceResult_.store54Returned){
                if(current.known!=15||current.bits()!=sourceResult_.argument54||
                   sourceResult_.before54.bytes!=snapshot.bytes||sourceResult_.before54.known!=snapshot.known)return false;
            }else if(current.bytes!=snapshot.bytes||current.known!=snapshot.known)return false;
            copy(0x20+4*lane,snapshot);
        }
        SourcePrefixWord one;one.store(1);copy(0x60,one);
        //Host backing zeros at pointer/padding bytes never become source words.
        return f.known==known&&f.bytes==expected&&*f.raw==f.bytes;
    }
    bool withdrawSourceType0ResourceNative()noexcept {
        auto& r=sourceResource_;if(!r.attempted||r.nativeWithdrawn)return true;
        if(sourceContactBorrower_)return false;
        if((sourceTransform_.attempted&&!sourceTransform_.nativeWithdrawn)||!preflightSourceType0ResourceNative())return false;
        if(r.pending){
            auto& resource=*r.pending;auto& f=resource.source_;
            if(resource.sourceAssociationBorrower_||resource.sourceResultBorrower_||resource.sourceDirectTransformBorrower_||sourceResult_.live)return false;
            //Explicit native cancellation, NOT source11AEE0/decrement history.
            //Original ref60 bytes and initialized receipt stay historical.
            resource.reference60_=0;f.raw.reset();f.nativeWithdrawn=true;
        }
        shape8C_->resource124.reset();r.nativeWithdrawn=true;
        //Retain native control/byte history until this owner dies. It no longer
        //owns raw source allocation or borrows live arrays/file after withdrawal.
        return true;
    }

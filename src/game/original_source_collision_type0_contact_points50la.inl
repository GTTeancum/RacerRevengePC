//Raw same-Type0 source view for296190; no semantic mesh or decoder authority.
    const void* sourceContactBorrower_=nullptr;
    const void* sourceContactHistory_=nullptr;
    bool sourceContactPinAttempted_=false,sourceContactNativeWithdrawn_=false;
    bool preflightSourceContactLeaseNative()const noexcept {
        if(!sourceContactPinAttempted_)return !sourceContactBorrower_&&!sourceContactHistory_&&!sourceContactNativeWithdrawn_;
        if(!sourceContactHistory_||sourceContactBorrower_!=(sourceContactNativeWithdrawn_?nullptr:sourceContactHistory_)||
           !sourceResource_.pending)return false;
        return sourceResource_.pending->sourceContactBorrower_==sourceContactBorrower_;
    }
    bool pinSourceContactPoints296190(const void* occurrence,const OriginalRacerSceneObject50LA* model)noexcept {
        if(!occurrence||sourceContactPinAttempted_||sourcePrefix_.nativeWithdrawn||sourceResource_.nativeWithdrawn||
           sourceTransform_.nativeWithdrawn||!sourceTransform_.returned||!sourceResult_.live||
           !sourceResource_.pending||sourceResource_.pending->sourceAssociationBorrower_!=sourcePrefix_.file.get()||
           sourcePrefix_.model!=model||!preflightSourceType0PrefixNative())return false;
        sourceContactPinAttempted_=true;sourceContactBorrower_=sourceContactHistory_=occurrence;
        sourceResource_.pending->sourceContactBorrower_=occurrence;return true;
    }
    bool sourceContactViewCurrentNative(const void* occurrence)const noexcept {
        return occurrence&&sourceContactBorrower_==occurrence&&!sourceContactNativeWithdrawn_&&
            !sourcePrefix_.nativeWithdrawn&&!sourceResource_.nativeWithdrawn&&sourceResource_.pending&&
            sourceResource_.pending->sourceAssociationBorrower_==sourcePrefix_.file.get()&&
            preflightSourceContactLeaseNative()&&
            sourceResource_.pending->sourceAssociationReady1326A0(sourcePrefix_.file.get(),this);
    }
    bool readSourceContactCount2962F0(const void* occurrence,SourcePrefixWord& output)const noexcept {
        if(!sourceContactViewCurrentNative(occurrence))return false;
        output=sourcePrefix_.vertex11C;return true; //Actual raw signed-loop operand, not native count.
    }
    bool readSourceContactPacked296200(const void* occurrence,SourcePrefixWord& output)const noexcept {
        if(!sourceContactViewCurrentNative(occurrence))return false;
        const auto& f=sourceResource_.pending->source_;
        if(!f.raw)return false; //Actual Resource124 dereference, not a safe NULL substitute.
        output={};
        for(unsigned i=0;i<4;++i){output.bytes[i]=(*f.raw)[0x18+i];
            if(f.known[0x18+i])output.known=std::uint8_t(output.known|(1u<<i));}
        return true;
    }
    bool readSourceContactVertex296224(const void* occurrence,std::uint32_t index,unsigned axis,
        bool packed,SourcePrefixWord& output)const noexcept {
        if(!sourceContactViewCurrentNative(occurrence)||axis>=3)return false;
        const auto& f=sourceResource_.pending->source_;const auto& a=sourceArrays_.vertices;
        if(!f.vertices10||!*f.vertices10||!a.raw||!a.payload||*f.vertices10!=*a.payload||
           a.stride!=(packed?6u:12u)||index>=a.count)return false;
        const auto width=packed?2u:4u;
        const auto offset=16u+std::size_t(index)*a.stride+axis*width;
        if(offset+width>a.bytes||a.known.size()!=a.bytes)return false;
        output={};
        for(unsigned i=0;i<width;++i){
            //Unknown raw bytes are preserved only as copy history; consumers
            //must require their mask before arithmetic or comparisons.
            output.bytes[i]=a.raw[offset+i];if(a.known[offset+i])output.known=std::uint8_t(output.known|(1u<<i));
        }
        return true;
    }
    bool unpinSourceContactPointsNative(const void* occurrence)noexcept {
        if(!sourceContactPinAttempted_)return true;
        if(occurrence!=sourceContactHistory_||!preflightSourceContactLeaseNative())return false;
        if(sourceContactNativeWithdrawn_)return true;
        if(!preflightSourceType0PrefixNative())return false;
        sourceResource_.pending->sourceContactBorrower_=nullptr;sourceContactBorrower_=nullptr;
        sourceContactNativeWithdrawn_=true;return true; //No source reference decrement or point rollback.
    }

// Private first actual AUB Sounds selection and genuine missing ambient count.
    bool sourceAmbientSoundsAttempted_=false,sourceAmbientSoundsReturned_=false;
    bool sourceAmbientCountAttempted_=false,sourceAmbientCountReturned_=false;
    bool sourceAmbientSoundsConfigCurrent()const noexcept {
        return thread_==std::this_thread::get_id()&&cohort_==Cohort::audioAub&&
            allocated_&&!sourceClosed_&&!nativeClosed_&&source_&&file_&&opens_==1&&
            attemptRetained_&&returned_==Return::one&&file_->current(*this,source_->file00)&&
            source_->binary08==1&&source_->opened0C==1&&source_->mode10==0&&
            source_->count14==74&&names_&&index_&&source_->names4CC==names_.get()&&
            source_->index4BC==index_.get()&&source_->indexBytes4C0==74u*8u&&
            namesProduced_==74&&indexProduced_==74u*8u&&sectionAttempts_==1&&sectionReturned_&&
            sourceAmbientSoundsAttempted_&&sourceAmbientSoundsReturned_&&
            !source_->text1C&&!source_->child24&&!source_->cacheCount438&&!lostBlock_&&
            source_->block4C4==block_.get()&&block_&&source_->field4C8==228&&
            source_->selected20==5&&sourceSectionPayloadCurrent(228)&&std::strcmp(source_->name28.data(),"Sounds")==0;
    }
    bool selectSourceAmbientSounds157D00(bool& returned,D& d)noexcept {
        d={};returned=false;sourcePC_=0x157D1C;
        if(sourceAmbientSoundsAttempted_||!sourceAudioConfigCurrent())return false;
        sourceAmbientSoundsAttempted_=true;++sectionAttempts_;
        const auto compare=[&](std::uint32_t i,int& result)noexcept {
            if(i>=74)return false;const auto* candidate=reinterpret_cast<const char*>(names_.get()+std::size_t(i)*1024);
            if(!std::memchr(candidate,0,1024))return false;result=std::strcmp("sounds",candidate);return true;
        };
        std::uint32_t low=0,high=73,mid=high/2;int compared=0;bool found=false;
        for(;;){sourcePC_=0x157B8C;if(!compare(mid,compared))return false;
            if(!compared){found=true;break;}if(compared>=0)low=mid;else high=mid;
            mid=low+(high-low)/2;if(high-low<2)break;}
        if(!found){sourcePC_=0x157BE8;if(!compare(low,compared))return false;
            if(!compared){mid=low;found=true;}
            else {sourcePC_=0x157C1C;if(!compare(high,compared))return false;if(!compared){mid=high;found=true;}}}
        if(!found){sourcePC_=0x157DD8;return true;} //Actual0; unsupported retained-cache continuation stays bounded.
        if(mid!=73)return false;
        sectionReturned_=false;
        std::memcpy(source_->name28.data(),"Sounds",6);source_->name28[6]=0;sourcePC_=0x157E9C;
        const auto offset=little(index_.get()+std::size_t(mid)*8),bytes=little(index_.get()+std::size_t(mid)*8+4);
        blockSectionBytes_=bytes;blockPayloadReturned_=false; //Actual index length, not4C8 read size.
        if(offset!=22279||bytes!=228){sourcePC_=0x157EB0;return false;}
        //Actual first selection sees constructorNULL4C4, so this is malloc,
        //not the later cache-growth realloc path. NULL publication is retained.
        sourcePC_=0x157EE4;source_->field4C8=bytes;
        sourcePC_=0x157EE8;block_.reset(new(std::nothrow) std::uint8_t[bytes]);blockKnownBytes_=0;
        sourcePC_=0x157EF0;source_->block4C4=block_.get();
        sourcePC_=0x157EF4;
        if(!file_->seek12F640(*this,source_->file00,offset,d)){sourcePC_=0x157F08;return false;}
        std::array<std::uint8_t,4> count{};std::size_t done=0;
        if(!readExact(count.data(),4,done,0x157F28,0x157F30,d))return false;
        source_->selected20=little(count.data());sourcePC_=0x157F38;
        if(!block_){sourcePC_=0x157F4C;return false;}
        if(!readSourceSectionPayload157F4C(d))return false;
        if(source_->selected20!=5)return false;
        sourceAmbientSoundsReturned_=true;sectionReturned_=true;returned=true;sourcePC_=0x15821C;return true;
    }
    bool readSourceAmbientCount158B30(std::int32_t& actualCount30,bool& returned,D& d)noexcept {
        d={};returned=false;sourcePC_=0x158B30;
        if(sourceAmbientCountAttempted_||!sourceAmbientSoundsConfigCurrent())return false;
        sourceAmbientCountAttempted_=true;
        //Actual158B48->158920->158934/158240 searches the five real fields.
        const auto compare=[&](std::uint32_t i,int& result)noexcept {
            if(i>=source_->selected20)return false;const auto* row=block_.get()+std::size_t(i)*43;
            if(!std::memchr(row,0,35))return false;
            result=std::strcmp("numambientsounds",reinterpret_cast<const char*>(row));return true;
        };
        std::uint32_t low=0,high=source_->selected20-1,mid=high/2;int compared=0;
        for(;;){sourcePC_=0x1583F8;if(!compare(mid,compared))return false;
            if(!compared){sourcePC_=0x158474;return false;}
            if(compared>=0)low=mid;else high=mid;mid=low+(high-low)/2;if(high-low<2)break;}
        sourcePC_=0x1584F0;if(!compare(low,compared))return false;
        if(!compared){sourcePC_=0x158560;return false;}
        sourcePC_=0x1585B0;if(!compare(high,compared))return false;
        if(!compared){sourcePC_=0x158620;return false;}
        sourcePC_=0x158640; //Actual raw getter0; parser returns0 at158A54.
        sourcePC_=0x158B60;actualCount30=0; //Actual a2=0 default through same owner lane.
        sourceAmbientCountReturned_=true;sourcePC_=0x158B78;return true; //Actual getter v0=0.
    }

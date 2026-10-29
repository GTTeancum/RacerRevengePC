// Included inside OriginalWorldSourceConfig50LA; exact same retained RA Config.
    bool sourceFogSelectionAttempted_=false,sourceFogSelectionReturned_=false;
    bool sourceFogConfigCurrent()const noexcept {
        return thread_==std::this_thread::get_id()&&source_&&!sourceClosed_&&!nativeClosed_&&
            sourceFogSelectionAttempted_&&sourceFogSelectionReturned_&&sectionReturned_&&sectionAttempts_==190&&
            source_->binary08==1&&source_->opened0C==1&&source_->mode10==0&&source_->selected20==3&&
            source_->field4C8==839&&source_->block4C4==block_.get()&&block_&&sourceSectionPayloadCurrent(147)&&!lostBlock_&&
            std::memcmp(source_->name28.data(),"Fog",4)==0&&file_&&file_->current(*this,source_->file00);
    }
    bool selectSourceFog157D00(bool& returned,D& d)noexcept {
        d={};returned=false;sourcePC_=0x157D1C;
        if(sourceFogSelectionAttempted_||sourceLightNextSelection_!=186||sectionAttempts_!=189||
           !sourceLightSelected_||*sourceLightSelected_!=185||!sourceLightConfigCurrent()||
           returned_!=Return::one||source_->count14!=1472||source_->names4CC!=names_.get()||
           source_->index4BC!=index_.get()||!names_||!index_||namesProduced_!=1472||indexProduced_!=1472*8||
           source_->child24||source_->text1C||source_->cacheCount438||source_->field4C8!=839)return false;
        sourceFogSelectionAttempted_=true;++sectionAttempts_;
        const auto compare=[&](std::uint32_t i,int& result)noexcept {
            if(i>=1472)return false;const auto* name=reinterpret_cast<const char*>(names_.get()+std::size_t(i)*1024);
            if(!std::memchr(name,0,1024))return false;result=std::strcmp("fog",name);return true;
        };
        std::uint32_t low=0,high=1471,mid=high/2;int compared=0;bool found=false;
        for(;;){sourcePC_=0x157B8C;if(!compare(mid,compared))return false;
            if(!compared){found=true;break;}if(compared>=0)low=mid;else high=mid;
            mid=low+(high-low)/2;if(high-low<2)break;}
        if(!found){sourcePC_=0x157BE8;if(!compare(low,compared))return false;
            if(!compared){mid=low;found=true;}
            else {sourcePC_=0x157C1C;if(!compare(high,compared))return false;if(!compared){mid=high;found=true;}}}
        if(!found){sourcePC_=0x157DD8;return true;} // Genuine return0 leaves selected Light185 unchanged.
        if(mid!=2)return false;sectionReturned_=false;
        std::memcpy(source_->name28.data(),"Fog",3);source_->name28[3]=0;sourcePC_=0x157E9C;
        const auto offset=little(index_.get()+16),bytes=little(index_.get()+20);
        blockSectionBytes_=bytes;blockPayloadReturned_=false; //Actual index length, not4C8 read size.
        if(offset!=38500||bytes!=147){sourcePC_=0x157EB0;return false;}
        // Actual existing839-byte block suffices: no malloc/realloc branch.
        sourcePC_=0x157EF4;
        if(!file_->seek12F640(*this,source_->file00,offset,d)){sourcePC_=0x157F08;return false;}
        std::array<std::uint8_t,4> count{};std::size_t done=0;
        if(!readExact(count.data(),4,done,0x157F28,0x157F30,d))return false;
        source_->selected20=little(count.data());sourcePC_=0x157F38;
        if(!readSourceSectionPayload157F4C(d))return false;
        if(source_->selected20!=3)return false;
        sectionReturned_=sourceFogSelectionReturned_=true;returned=true;sourcePC_=0x15821C;return true;
    }
    bool readSourceFogField158240(const char* lower,std::string_view& value,bool& found,
        std::uint32_t& reachedCopyPC,D& d)noexcept {
        d={};value={};found=false;reachedCopyPC=0;sourcePC_=0x158280;
        if(!sourceFogConfigCurrent()||!lower||
           (std::strcmp(lower,"colorrgb")&&std::strcmp(lower,"visibility")&&std::strcmp(lower,"haziness")))return false;
        const auto compare=[&](std::uint32_t i,int& result)noexcept {
            if(i>=3)return false;const auto* row=block_.get()+std::size_t(i)*43;
            if(!std::memchr(row,0,35))return false;result=std::strcmp(lower,reinterpret_cast<const char*>(row));return true;
        };
        const auto capture=[&](std::uint32_t i)noexcept {
            const auto offset=little(block_.get()+std::size_t(i)*43+35);if(offset>=147)return false;
            const auto* begin=reinterpret_cast<const char*>(block_.get()+offset);
            const auto* end=static_cast<const char*>(std::memchr(begin,0,147-offset));if(!end)return false;
            reachedCopyPC=sourcePC_==0x1583F8?0x158474:sourcePC_==0x1584F0?0x158560:0x158620;
            fieldCopyPC_=reachedCopyPC;value=std::string_view(begin,std::size_t(end-begin));found=true;return true;
        };
        std::uint32_t low=0,high=2,mid=1;int compared=0;
        for(;;){sourcePC_=0x1583F8;if(!compare(mid,compared))return false;if(!compared)return capture(mid);
            if(compared>=0)low=mid;else high=mid;mid=low+(high-low)/2;if(high-low<2)break;}
        sourcePC_=0x1584F0;if(!compare(low,compared))return false;if(!compared)return capture(low);
        sourcePC_=0x1585B0;if(!compare(high,compared))return false;if(!compared)return capture(high);
        sourcePC_=0x158640;return true; // Actual destination untouched; wrapper owns default stores.
    }

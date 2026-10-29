//Included privately inside OriginalWorldSourceConfig50LA. Same real TOB713
//name/index arrays and source FileManager handle, no decoded metadata selector.
    enum class ObjectSectionPhase:std::uint8_t {unreached,podAttempted,podSelected,rainAttempted,rainMissing,animationsAttempted,animationsMissing};
    ObjectSectionPhase objectSectionPhase_=ObjectSectionPhase::unreached;
    std::uint8_t objectPodReads_=0;
    bool sourceObjectDirectoryCurrent()const noexcept {
        return thread_==std::this_thread::get_id()&&cohort_==Cohort::objectsTob&&
            allocated_&&!sourceClosed_&&!nativeClosed_&&source_&&file_&&opens_==1&&
            attemptRetained_&&returned_==Return::one&&file_->current(*this,source_->file00)&&
            source_->binary08==1&&source_->opened0C==1&&source_->mode10==0&&
            source_->count14==713&&names_&&index_&&source_->names4CC==names_.get()&&
            source_->index4BC==index_.get()&&source_->indexBytes4C0==713u*8u&&
            namesProduced_==713&&indexProduced_==713u*8u&&!source_->text1C&&
            !source_->child24&&!source_->cacheCount438&&!lostBlock_;
    }
    bool sourceObjectPodCurrent()const noexcept {
        return sourceObjectDirectoryCurrent()&&sectionReturned_&&source_->selected20==3&&
            source_->field4C8==135&&source_->block4C4==block_.get()&&block_&&sourceSectionPayloadCurrent(135)&&
            std::memcmp(source_->name28.data(),"PodRacer",9)==0;
    }
    bool selectSourceObjectSection157D00(std::string_view name,bool& returned,D& d)noexcept {
        d={};returned=false;sourcePC_=0x157D1C;
        const char* lower=nullptr;bool pod=false;
        if(name=="PodRacer"){
            if(objectSectionPhase_!=ObjectSectionPhase::unreached||!sourceObjectConfigCurrent())return false;
            lower="podracer";pod=true;objectSectionPhase_=ObjectSectionPhase::podAttempted;
        }else if(name=="Rain"){
            if(objectSectionPhase_!=ObjectSectionPhase::podSelected||objectPodReads_!=2||
               sectionAttempts_!=1||!sourceObjectPodCurrent())return false;
            lower="rain";objectSectionPhase_=ObjectSectionPhase::rainAttempted;
        }else if(name=="321_Anims"){
            if(!sourceObjectPodCurrent()||objectPodReads_!=2||
               !((objectSectionPhase_==ObjectSectionPhase::podSelected&&sectionAttempts_==1)||
                 (objectSectionPhase_==ObjectSectionPhase::rainMissing&&sectionAttempts_==2)))return false;
            lower="321_anims";objectSectionPhase_=ObjectSectionPhase::animationsAttempted;
        }else return false;
        ++sectionAttempts_;
        const auto compare=[&](std::uint32_t i,int& result)noexcept {
            if(i>=713)return false;const char* value=reinterpret_cast<const char*>(names_.get()+std::size_t(i)*1024);
            if(!std::memchr(value,0,1024))return false;result=std::strcmp(lower,value);return true;
        };
        std::uint32_t low=0,high=712,mid=high/2;int compared=0;bool found=false;
        for(;;){sourcePC_=0x157B8C;if(!compare(mid,compared))return false;
            if(!compared){found=true;break;}if(compared>=0)low=mid;else high=mid;
            mid=low+(high-low)/2;if(high-low<2)break;}
        if(!found){sourcePC_=0x157BE8;if(!compare(low,compared))return false;
            if(!compared){mid=low;found=true;}else{sourcePC_=0x157C1C;if(!compare(high,compared))return false;if(!compared){mid=high;found=true;}}}
        if(!found){
            //Actual negative selection leaves selected name/count/block AND
            //numeric file cursor unchanged; no forged empty cache/section.
            if(name=="Rain")objectSectionPhase_=ObjectSectionPhase::rainMissing;
            if(name=="321_Anims")objectSectionPhase_=ObjectSectionPhase::animationsMissing;
            sourcePC_=0x157DD8;return true;
        }
        sourcePC_=0x157E90;
        if(!pod||mid!=686)return false; //Unexpected positive Rain/anims bounded before unjoined cache effects.
        sectionReturned_=false;
        std::memcpy(source_->name28.data(),"PodRacer",8);source_->name28[8]=0;sourcePC_=0x157E9C;
        const auto offset=little(index_.get()+std::size_t(mid)*8),bytes=little(index_.get()+std::size_t(mid)*8+4);
        blockSectionBytes_=bytes;blockPayloadReturned_=false; //Actual index length, not4C8 read size.
        if(offset!=89856||bytes!=135){sourcePC_=0x157EB0;return false;}
        //First selected TOB section. Real4C8 size precedes nullable malloc,
        //real4C4 publication precedes seek/read/count and first NULL payload use.
        if(source_->block4C4||block_||source_->field4C8)return false;
        source_->field4C8=bytes;sourcePC_=0x157EE4;
        block_.reset(new(std::nothrow) std::uint8_t[bytes]);blockKnownBytes_=0;
        source_->block4C4=block_.get();sourcePC_=0x157EF0;
        if(!file_->seek12F640(*this,source_->file00,offset,d)){sourcePC_=0x157F08;return false;}
        std::array<std::uint8_t,4> count{};std::size_t done=0;
        if(!readExact(count.data(),4,done,0x157F28,0x157F30,d))return false;
        source_->selected20=little(count.data());sourcePC_=0x157F38;
        if(!block_){sourcePC_=0x157F4C;return false;}
        if(!readSourceSectionPayload157F4C(d))return false;
        if(source_->selected20!=3)return false;
        sectionReturned_=true;objectSectionPhase_=ObjectSectionPhase::podSelected;
        returned=true;sourcePC_=0x15821C;return true;
    }
    bool readSourceObjectInteger158B30(std::string_view key,std::uint32_t defaultValue,
        std::uint32_t& actualOutput,bool& returned,D& d)noexcept {
        d={};returned=false;sourcePC_=0x158280;
        if(objectSectionPhase_!=ObjectSectionPhase::podSelected||sectionAttempts_!=1||!sourceObjectPodCurrent())return false;
        const char* lower=nullptr;const char* exact=nullptr;
        if(objectPodReads_==0&&key=="TrackEmitsDust"&&defaultValue==0){lower="trackemitsdust";exact="1";}
        else if(objectPodReads_==1&&key=="IPUCacheSize"&&defaultValue==3){lower="ipucachesize";exact="2";}
        else return false;
        const auto compare=[&](std::uint32_t i,int& result)noexcept {
            if(i>=3)return false;const auto* row=block_.get()+std::size_t(i)*43;
            if(!std::memchr(row,0,35))return false;result=std::strcmp(lower,reinterpret_cast<const char*>(row));return true;
        };
        std::string_view value;
        const auto capture=[&](std::uint32_t i)noexcept {
            const auto offset=little(block_.get()+std::size_t(i)*43+35);if(offset>=sourceSelectedKnownBytes())return false;
            const auto* start=reinterpret_cast<const char*>(block_.get()+offset);
            const auto* end=static_cast<const char*>(std::memchr(start,0,sourceSelectedKnownBytes()-offset));if(!end)return false;
            fieldCopyPC_=sourcePC_==0x1583F8?0x158474:sourcePC_==0x1584F0?0x158560:0x158620;
            value=std::string_view(start,std::size_t(end-start));return true;
        };
        std::uint32_t low=0,high=2,mid=1;int compared=0;bool found=false;
        for(;;){sourcePC_=0x1583F8;if(!compare(mid,compared))return false;
            if(!compared){found=true;break;}if(compared>=0)low=mid;else high=mid;
            mid=low+(high-low)/2;if(high-low<2)break;}
        if(!found){sourcePC_=0x1584F0;if(!compare(low,compared))return false;
            if(!compared){mid=low;found=true;}else{sourcePC_=0x1585B0;if(!compare(high,compared))return false;if(!compared){mid=high;found=true;}}}
        if(found){if(!capture(mid))return false;
            //Exact small decimal tokens reach actual30AE70 and158A44. No
            //host parser, gameplay flag policy, or guessed integer conversion.
            sourcePC_=0x158A34;if(value!=exact)return false;
            sourcePC_=0x158A44;actualOutput=key=="TrackEmitsDust"?1u:2u;returned=true;
        }else{sourcePC_=0x158B60;actualOutput=defaultValue;}
        ++objectPodReads_;sourcePC_=0x158B68;return true;
    }

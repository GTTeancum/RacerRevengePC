// Private real TOB Trigger two-row selections. Tuples qualify reads, never replace payload.
    struct SourceTriggerSection {const char* name;const char* lower;std::uint32_t ordinal,offset,bytes,count;};
    inline static constexpr SourceTriggerSection sourceTriggerSections_[]={
        {"Triggers","triggers",703,94429,45,1},
        {"Trigger1","trigger1",699,93265,478,9},
        {"Trigger1Action1","trigger1action1",700,93747,96,2},
        {"Trigger2","trigger2",701,93847,477,9},
        {"Trigger2Action1","trigger2action1",702,94328,97,2},
    };
    std::uint32_t sourceTriggerNextSelection_=0,sourceTriggerBaseAttempt_=0;
    std::optional<std::uint32_t> sourceTriggerSelected_;
    bool sourceTriggerConfigCurrent()const noexcept {
        if(!sourceTriggerSelected_||*sourceTriggerSelected_>=5)return false;
        const auto& expected=sourceTriggerSections_[*sourceTriggerSelected_];
        return sourceObjectDirectoryCurrent()&&sectionReturned_&&sourceTriggerNextSelection_&&
            sectionAttempts_==sourceTriggerBaseAttempt_+sourceTriggerNextSelection_&&
            source_->selected20==expected.count&&source_->field4C8==(*sourceTriggerSelected_?478u:231u)&&
            source_->block4C4==block_.get()&&block_&&sourceSectionPayloadCurrent(expected.bytes)&&
            std::strcmp(source_->name28.data(),expected.name)==0;
    }
    bool selectSourceTriggerSection157D00(std::uint32_t selected,bool& returned,D& d)noexcept {
        d={};returned=false;sourcePC_=0x157D1C;
        if(selected>=5||selected!=sourceTriggerNextSelection_)return false;
        if(!selected){
            if(!sourceToyAnimationsMissingCurrent())return false;
            sourceTriggerBaseAttempt_=sectionAttempts_;
        }else if(!sourceTriggerConfigCurrent())return false;
        if(!sourceObjectDirectoryCurrent()||sectionAttempts_!=sourceTriggerBaseAttempt_+selected)return false;
        ++sourceTriggerNextSelection_;++sectionAttempts_;
        const auto& expected=sourceTriggerSections_[selected];
        const auto compare=[&](std::uint32_t i,int& result)noexcept {
            if(i>=713)return false;const auto* candidate=reinterpret_cast<const char*>(names_.get()+std::size_t(i)*1024);
            if(!std::memchr(candidate,0,1024))return false;result=std::strcmp(expected.lower,candidate);return true;
        };
        std::uint32_t low=0,high=712,mid=high/2;int compared=0;bool found=false;
        for(;;){sourcePC_=0x157B8C;if(!compare(mid,compared))return false;
            if(!compared){found=true;break;}if(compared>=0)low=mid;else high=mid;
            mid=low+(high-low)/2;if(high-low<2)break;}
        if(!found){sourcePC_=0x157BE8;if(!compare(low,compared))return false;
            if(!compared){mid=low;found=true;}
            else {sourcePC_=0x157C1C;if(!compare(high,compared))return false;if(!compared){mid=high;found=true;}}}
        if(!found){sourcePC_=0x157DD8;return true;} // Actual0; caller must bound before unjoined missing-section continuation, never grant later selection.
        if(mid!=expected.ordinal)return false;
        sectionReturned_=false;const auto length=std::strlen(expected.name);
        std::memcpy(source_->name28.data(),expected.name,length);source_->name28[length]=0;sourcePC_=0x157E9C;
        const auto offset=little(index_.get()+std::size_t(mid)*8),bytes=little(index_.get()+std::size_t(mid)*8+4);
        blockSectionBytes_=bytes;blockPayloadReturned_=false; //Actual index length, not4C8 read size.
        if(offset!=expected.offset||bytes!=expected.bytes){sourcePC_=0x157EB0;return false;}
        // Same native lowering of actual realloc: source capacity first, NULL publication
        // unconditional, old backing retained separately only for native withdrawal.
        if(source_->field4C8<bytes){
            const auto oldBytes=source_->field4C8;source_->field4C8=bytes;sourcePC_=0x157ECC;
            std::unique_ptr<std::uint8_t[]> next(new(std::nothrow) std::uint8_t[bytes]);
            if(next){std::memcpy(next.get(),block_.get(),oldBytes);block_=std::move(next);}
            else {lostBlock_=std::move(block_);lostBlockKnownBytes_=blockKnownBytes_;blockKnownBytes_=0;}
            source_->block4C4=block_.get();sourcePC_=0x157EE0;
        }
        sourcePC_=0x157EF4;
        if(!file_->seek12F640(*this,source_->file00,offset,d)){sourcePC_=0x157F08;return false;}
        std::array<std::uint8_t,4> count{};std::size_t done=0;
        if(!readExact(count.data(),4,done,0x157F28,0x157F30,d))return false;
        source_->selected20=little(count.data());sourcePC_=0x157F38;
        if(!block_){sourcePC_=0x157F4C;return false;}
        if(!readSourceSectionPayload157F4C(d))return false;
        if(source_->selected20!=expected.count)return false;
        sourceTriggerSelected_=selected;
        sectionReturned_=true;returned=true;sourcePC_=0x15821C;return true;
    }
    bool readSourceTriggerField158240(const char* lower,std::string_view& value,bool& found,
        std::uint32_t& reachedCopyPC,D& d)noexcept {
        d={};value={};found=false;reachedCopyPC=0;sourcePC_=0x158280;
        if(!sourceTriggerConfigCurrent()||!lower)return false;
        bool admitted=false;
        if(*sourceTriggerSelected_==0)admitted=std::strcmp(lower,"numberoftriggers")==0;
        else if(*sourceTriggerSelected_==1||*sourceTriggerSelected_==3){
            static constexpr const char* keys[]={"name","numberofactions","zone","ignoretrigger","startenabled","parent","randomtriggerpercent",
                "position","box","yaw","pitch","roll","radius","onetimeonly"};
            for(const auto* key:keys)if(std::strcmp(lower,key)==0){admitted=true;break;}
        }else {
            static constexpr const char* keys[]={"actiontype","targetnodeindex","reset","specificobject"};
            for(const auto* key:keys)if(std::strcmp(lower,key)==0){admitted=true;break;}
        }
        if(!admitted)return false;
        const auto compare=[&](std::uint32_t i,int& result)noexcept {
            if(i>=source_->selected20)return false;const auto* row=block_.get()+std::size_t(i)*43;
            if(!std::memchr(row,0,35))return false;result=std::strcmp(lower,reinterpret_cast<const char*>(row));return true;
        };
        const auto capture=[&](std::uint32_t i)noexcept {
            const auto offset=little(block_.get()+std::size_t(i)*43+35);if(offset>=sourceSelectedKnownBytes())return false;
            const auto* begin=reinterpret_cast<const char*>(block_.get()+offset);
            const auto* end=static_cast<const char*>(std::memchr(begin,0,sourceSelectedKnownBytes()-offset));if(!end)return false;
            reachedCopyPC=sourcePC_==0x1583F8?0x158474:sourcePC_==0x1584F0?0x158560:0x158620;
            fieldCopyPC_=reachedCopyPC;value=std::string_view(begin,std::size_t(end-begin));found=true;return true;
        };
        std::uint32_t low=0,high=source_->selected20-1,mid=high/2;int compared=0;
        for(;;){sourcePC_=0x1583F8;if(!compare(mid,compared))return false;if(!compared)return capture(mid);
            if(compared>=0)low=mid;else high=mid;mid=low+(high-low)/2;if(high-low<2)break;}
        sourcePC_=0x1584F0;if(!compare(low,compared))return false;if(!compared)return capture(low);
        sourcePC_=0x1585B0;if(!compare(high,compared))return false;if(!compared)return capture(high);
        sourcePC_=0x158640;return true;
    }

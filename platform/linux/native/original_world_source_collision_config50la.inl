// Included inside the SAME OriginalWorldSourceConfig50LA, after StaticModel transport.
// Private second pass for actual most-derived World v8C collision descriptors.
// The immutable sourceStaticModelSections_ table qualifies the same authored sections;
// it is not source payload and cannot replace any actual seek/count/read below.
    std::uint32_t sourceCollisionDescriptorNextSelection_=0;
    std::optional<std::uint32_t> sourceCollisionDescriptorSelected_;
    std::array<char,32> sourceCollisionDescriptorSelectedName_{}; // Native currentness only.
    bool sourceCollisionDescriptorConfigCurrent()const noexcept {
        if(!sourceCollisionDescriptorSelected_||*sourceCollisionDescriptorSelected_>=1261)return false;
        const auto& expected=sourceStaticModelSections_[*sourceCollisionDescriptorSelected_];
        return thread_==std::this_thread::get_id()&&source_&&!sourceClosed_&&!nativeClosed_&&sectionReturned_&&
            sourceCollisionDescriptorNextSelection_&&sectionAttempts_==1482u+sourceCollisionDescriptorNextSelection_&&
            source_->binary08==1&&source_->opened0C==1&&source_->mode10==0&&
            source_->selected20==expected.count&&source_->field4C8==839&&
            source_->block4C4==block_.get()&&block_&&sourceSectionPayloadCurrent(expected.bytes)&&!lostBlock_&&
            std::strcmp(source_->name28.data(),sourceCollisionDescriptorSelectedName_.data())==0&&
            file_&&file_->current(*this,source_->file00);
    }
    bool selectSourceCollisionDescriptor157D00(std::uint32_t zone,std::uint32_t model,bool& returned,D& d)noexcept {
        d={};returned=false;sourcePC_=0x157D1C;
        if(zone>=10||model>=sourceStaticModelCounts_[zone])return false;
        const auto selected=sourceStaticModelOffsets_[zone]+model;
        if(selected!=sourceCollisionDescriptorNextSelection_||sectionAttempts_!=1482u+selected)return false;
        if(!selected){
            if(sourceStaticModelNextSelection_!=1261||!sourceStaticModelSelected_||
               *sourceStaticModelSelected_!=1260||!sourceStaticModelConfigCurrent())return false;
        }
        else if(!sourceCollisionDescriptorConfigCurrent())return false;
        if(returned_!=Return::one||source_->count14!=1472||source_->names4CC!=names_.get()||
           source_->index4BC!=index_.get()||!names_||!index_||namesProduced_!=1472||indexProduced_!=1472*8||
           source_->child24||source_->text1C||source_->cacheCount438||source_->field4C8!=839)return false;
        ++sourceCollisionDescriptorNextSelection_;++sectionAttempts_;
        std::array<char,32> name{};char* at=name.data(); // Native scratch, no source-tail publication.
        const auto decimal=[&](std::uint32_t n)noexcept {
            char reverse[10];unsigned used=0;do{reverse[used++]=char('0'+n%10);n/=10;}while(n);
            while(used)*at++=reverse[--used];
        };
        std::memcpy(at,"Zone",4);at+=4;decimal(zone+1);
        std::memcpy(at,"-Model",6);at+=6;decimal(model+1);*at=0;
        auto lower=name;
        for(auto* c=lower.data();*c;++c)if(*c>='A'&&*c<='Z')*c=char(*c-'A'+'a');
        const auto compare=[&](std::uint32_t i,int& result)noexcept {
            if(i>=1472)return false;const auto* candidate=reinterpret_cast<const char*>(names_.get()+std::size_t(i)*1024);
            if(!std::memchr(candidate,0,1024))return false;result=std::strcmp(lower.data(),candidate);return true;
        };
        std::uint32_t low=0,high=1471,mid=high/2;int compared=0;bool found=false;
        for(;;){sourcePC_=0x157B8C;if(!compare(mid,compared))return false;
            if(!compared){found=true;break;}if(compared>=0)low=mid;else high=mid;
            mid=low+(high-low)/2;if(high-low<2)break;}
        if(!found){sourcePC_=0x157BE8;if(!compare(low,compared))return false;
            if(!compared){mid=low;found=true;}
            else {sourcePC_=0x157C1C;if(!compare(high,compared))return false;if(!compared){mid=high;found=true;}}}
        if(!found){sourcePC_=0x157DD8;return true;} // Actual0; caller must bound before missing collision-object branch, never skip an unknown slot.
        const auto& expected=sourceStaticModelSections_[selected];if(mid!=expected.ordinal)return false;
        sectionReturned_=false;const auto length=std::strlen(name.data());
        std::memcpy(source_->name28.data(),name.data(),length);source_->name28[length]=0;sourcePC_=0x157E9C;
        const auto offset=little(index_.get()+std::size_t(mid)*8),bytes=little(index_.get()+std::size_t(mid)*8+4);
        blockSectionBytes_=bytes;blockPayloadReturned_=false; //Actual index length, not4C8 read size.
        if(offset!=expected.offset||bytes!=expected.bytes){sourcePC_=0x157EB0;return false;}
        // Existing839-byte source capacity suffices for all authenticated model sections (max508 bytes).
        // The positive selection really seeks and rereads.
        sourcePC_=0x157EF4;
        if(!file_->seek12F640(*this,source_->file00,offset,d)){sourcePC_=0x157F08;return false;}
        std::array<std::uint8_t,4> count{};std::size_t done=0;
        if(!readExact(count.data(),4,done,0x157F28,0x157F30,d))return false;
        source_->selected20=little(count.data());sourcePC_=0x157F38;
        if(!readSourceSectionPayload157F4C(d))return false;
        if(source_->selected20!=expected.count)return false;
        sourceCollisionDescriptorSelected_=selected;sourceCollisionDescriptorSelectedName_=name;
        sectionReturned_=true;returned=true;sourcePC_=0x15821C;return true;
    }
    bool readSourceCollisionDescriptorField158240(const char* lower,std::string_view& value,bool& found,
        std::uint32_t& reachedCopyPC,D& d)noexcept {
        d={};value={};found=false;reachedCopyPC=0;sourcePC_=0x158280;
        if(!sourceCollisionDescriptorConfigCurrent()||!lower)return false;
        bool admitted=false;
        static constexpr const char* keys[]={"statictype","slt","position","lookvector","upvector","uselighting",
            "usedynamiclighting","issmoker","smokeoffset","smokevelocity","col","materialid"};
        for(const auto* key:keys)if(std::strcmp(lower,key)==0){admitted=true;break;}
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

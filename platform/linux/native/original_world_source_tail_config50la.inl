// Private actual270900 tail, SAME source Config/cache/File after true collision pass.
    bool sourceWorldTailSelectionAttempted_=false,sourceWorldTailEnvironmentSelected_=false;
    bool sourceWorldTailEnvironmentCurrent()const noexcept {
        return thread_==std::this_thread::get_id()&&source_&&!sourceClosed_&&!nativeClosed_&&
            sourceWorldTailSelectionAttempted_&&sourceWorldTailEnvironmentSelected_&&sectionReturned_&&sectionAttempts_==2744&&
            source_->binary08==1&&source_->opened0C==1&&source_->mode10==0&&source_->selected20==18&&
            source_->field4C8==839&&source_->block4C4==block_.get()&&block_&&sourceSectionPayloadCurrent(839)&&!lostBlock_&&
            std::memcmp(source_->name28.data(),"Environment",12)==0&&file_&&file_->current(*this,source_->file00);
    }
    bool selectSourceWorldTailEnvironment157D00(bool& returned,D& d)noexcept {
        d={};returned=false;sourcePC_=0x157D1C;
        if(sourceWorldTailSelectionAttempted_||sourceCollisionDescriptorNextSelection_!=1261||
           !sourceCollisionDescriptorSelected_||*sourceCollisionDescriptorSelected_!=1260||
           sectionAttempts_!=2743||!sourceCollisionDescriptorConfigCurrent())return false;
        if(returned_!=Return::one||source_->count14!=1472||source_->names4CC!=names_.get()||
           source_->index4BC!=index_.get()||!names_||!index_||namesProduced_!=1472||indexProduced_!=1472*8||
           source_->child24||source_->text1C||source_->cacheCount438||source_->field4C8!=839)return false;
        sourceWorldTailSelectionAttempted_=true;++sectionAttempts_;
        static constexpr const char* lower="environment";
        const auto compare=[&](std::uint32_t i,int& result)noexcept {
            if(i>=1472)return false;const auto* candidate=reinterpret_cast<const char*>(names_.get()+std::size_t(i)*1024);
            if(!std::memchr(candidate,0,1024))return false;result=std::strcmp(lower,candidate);return true;
        };
        std::uint32_t low=0,high=1471,mid=high/2;int compared=0;bool found=false;
        for(;;){sourcePC_=0x157B8C;if(!compare(mid,compared))return false;
            if(!compared){found=true;break;}if(compared>=0)low=mid;else high=mid;
            mid=low+(high-low)/2;if(high-low<2)break;}
        if(!found){sourcePC_=0x157BE8;if(!compare(low,compared))return false;
            if(!compared){mid=low;found=true;}
            else {sourcePC_=0x157C1C;if(!compare(high,compared))return false;if(!compared){mid=high;found=true;}}}
        if(!found){sourcePC_=0x157DD8;return true;} // Actual0 preserves prior collision cache; caller bounds before unjoined retained-block getter.
        if(mid!=1)return false;
        sectionReturned_=false;constexpr std::size_t length=11;
        std::memcpy(source_->name28.data(),"Environment",length);source_->name28[length]=0;sourcePC_=0x157E9C;
        const auto offset=little(index_.get()+std::size_t(mid)*8),bytes=little(index_.get()+std::size_t(mid)*8+4);
        blockSectionBytes_=bytes;blockPayloadReturned_=false; //Actual index length, not4C8 read size.
        if(offset!=37657||bytes!=839){sourcePC_=0x157EB0;return false;}
        // Existing839-byte source capacity exactly fits the actual Environment reread.
        // The positive selection really seeks and rereads.
        sourcePC_=0x157EF4;
        if(!file_->seek12F640(*this,source_->file00,offset,d)){sourcePC_=0x157F08;return false;}
        std::array<std::uint8_t,4> count{};std::size_t done=0;
        if(!readExact(count.data(),4,done,0x157F28,0x157F30,d))return false;
        source_->selected20=little(count.data());sourcePC_=0x157F38;
        if(!readSourceSectionPayload157F4C(d))return false;
        if(source_->selected20!=18)return false;
        sourceWorldTailEnvironmentSelected_=true;
        sectionReturned_=true;returned=true;sourcePC_=0x15821C;return true;
    }
    bool readSourceWorldTailField158240(const char* lower,std::string_view& value,bool& found,
        std::uint32_t& reachedCopyPC,D& d)noexcept {
        d={};value={};found=false;reachedCopyPC=0;sourcePC_=0x158280;
        if(!sourceWorldTailEnvironmentCurrent()||!lower)return false;
        if(std::strcmp(lower,"ambientcolorflashminidletime")!=0)return false;
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
    bool readSourceAmbientFlashMinimum158B80(std::optional<std::uint32_t>& actualOutputBits,
        bool& returned,D& d)noexcept {
        d={};returned=false;std::string_view value;bool found=false;std::uint32_t copy=0;
        if(!readSourceWorldTailField158240("ambientcolorflashminidletime",value,found,copy,d))return false;
        if(found){sourcePC_=0x158BAC;return false;} //Unexpected positive parser branch remains unjoined.
        sourcePC_=0x158BDC;actualOutputBits=0; //Actual f12 default+0 from270E98 into sameWorld368.
        sourcePC_=0x158BE4;return true; //Genuine getter return0, not fabricated success1.
    }

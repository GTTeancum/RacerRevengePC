// Included inside same Config owner. Actual missing InstanceMasters lookup
// retains Portal10 source selection; it is NOT a decoded empty-world shortcut.
    bool sourceInstanceMasterSelectionAttempted_=false,sourceInstanceMasterPositiveFound_=false;
    bool sourceInstanceMasterConfigCurrent()const noexcept {
        if(!sourceInstanceMasterSelectionAttempted_||sourceInstanceMasterPositiveFound_||
           sourcePortalNextSelection_!=10||!sourcePortalSelected_||*sourcePortalSelected_!=9||
           sectionAttempts_!=221)return false;
        const auto& expected=sourcePortalSections_[9];
        return thread_==std::this_thread::get_id()&&source_&&!sourceClosed_&&!nativeClosed_&&sectionReturned_&&
            source_->binary08==1&&source_->opened0C==1&&source_->mode10==0&&
            source_->selected20==expected.count&&source_->field4C8==839&&
            source_->block4C4==block_.get()&&block_&&sourceSectionPayloadCurrent(expected.bytes)&&!lostBlock_&&
            std::strcmp(source_->name28.data(),"Portal10")==0&&
            file_&&file_->current(*this,source_->file00);
    }
    bool selectSourceInstanceMasters157D00(bool& returned,D& d)noexcept {
        d={};returned=false;sourcePC_=0x157D1C;
        if(sourceInstanceMasterSelectionAttempted_||sourcePortalNextSelection_!=10||
           !sourcePortalSelected_||*sourcePortalSelected_!=9||sectionAttempts_!=220||
           !sourcePortalConfigCurrent()||returned_!=Return::one||source_->count14!=1472||
           source_->names4CC!=names_.get()||source_->index4BC!=index_.get()||!names_||!index_||
           namesProduced_!=1472||indexProduced_!=1472*8||source_->child24||source_->text1C||source_->cacheCount438)
            return false;
        sourceInstanceMasterSelectionAttempted_=true;++sectionAttempts_;
        const auto compare=[&](std::uint32_t i,int& result)noexcept {
            if(i>=1472)return false;const auto* candidate=reinterpret_cast<const char*>(names_.get()+std::size_t(i)*1024);
            if(!std::memchr(candidate,0,1024))return false;result=std::strcmp("instancemasters",candidate);return true;
        };
        std::uint32_t low=0,high=1471,mid=high/2;int compared=0;bool found=false;
        for(;;){sourcePC_=0x157B8C;if(!compare(mid,compared))return false;
            if(!compared){found=true;break;}if(compared>=0)low=mid;else high=mid;
            mid=low+(high-low)/2;if(high-low<2)break;}
        if(!found){sourcePC_=0x157BE8;if(!compare(low,compared))return false;
            if(!compared){mid=low;found=true;}
            else {sourcePC_=0x157C1C;if(!compare(high,compared))return false;if(!compared){mid=high;found=true;}}}
        if(!found){sourcePC_=0x157DD8;return true;} //Actual0; no name/cache/count/read effects.
        //Positive branch is not present in authenticated RA. Bound before its
        //name publication instead of treating an unexpected section as absent.
        sourceInstanceMasterPositiveFound_=true;sourcePC_=0x157E9C;return false;
    }
    bool readSourceInstanceMasterField158240(const char* lower,std::string_view& value,bool& found,
        std::uint32_t& reachedCopyPC,D& d)noexcept {
        d={};value={};found=false;reachedCopyPC=0;sourcePC_=0x158280;
        if(!sourceInstanceMasterConfigCurrent()||!lower)return false;
        if(std::strcmp(lower,"numberofinstancemasters")!=0)return false;
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

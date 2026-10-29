// Included inside same OriginalWorldSourceConfig50LA. Private RA Portal-only transport.
    struct SourcePortalSection {std::uint32_t ordinal,offset,bytes,count;};
    static constexpr std::array<SourcePortalSection,10> sourcePortalSections_{{
        {190u,118947u,410u,6u}, // portal1
        {192u,119772u,412u,6u}, // portal2
        {193u,120188u,410u,6u}, // portal3
        {194u,120602u,408u,6u}, // portal4
        {195u,121014u,406u,6u}, // portal5
        {196u,121424u,406u,6u}, // portal6
        {197u,121834u,411u,6u}, // portal7
        {198u,122249u,410u,6u}, // portal8
        {199u,122663u,406u,6u}, // portal9
        {191u,119361u,407u,6u}, // portal10
    }};
    std::uint32_t sourcePortalNextSelection_=0;
    std::optional<std::uint32_t> sourcePortalSelected_;
    std::array<char,10> sourcePortalSelectedName_{}; // Native currentness only.
    bool sourcePortalConfigCurrent()const noexcept {
        if(!sourcePortalSelected_||*sourcePortalSelected_>=10)return false;
        const auto& expected=sourcePortalSections_[*sourcePortalSelected_];
        return thread_==std::this_thread::get_id()&&source_&&!sourceClosed_&&!nativeClosed_&&sectionReturned_&&
            sourcePortalNextSelection_&&sectionAttempts_==210u+sourcePortalNextSelection_&&
            source_->binary08==1&&source_->opened0C==1&&source_->mode10==0&&
            source_->selected20==expected.count&&source_->field4C8==839&&
            source_->block4C4==block_.get()&&block_&&sourceSectionPayloadCurrent(expected.bytes)&&!lostBlock_&&
            std::strcmp(source_->name28.data(),sourcePortalSelectedName_.data())==0&&
            file_&&file_->current(*this,source_->file00);
    }
    bool selectSourcePortal157D00(std::uint32_t index,bool& returned,D& d)noexcept {
        d={};returned=false;sourcePC_=0x157D1C;
        if(index>=10||sourcePortalNextSelection_!=index||
           sectionAttempts_!=210u+sourcePortalNextSelection_)return false;
        if(sourcePortalNextSelection_==0){
            if(sourceZoneNextSelection_!=20||!sourceZoneSelected_||*sourceZoneSelected_!=9||
               sourceZoneSelectedPass_!=1||!sourceZoneConfigCurrent())return false;
        }else if(!sourcePortalConfigCurrent())return false;
        if(returned_!=Return::one||source_->count14!=1472||source_->names4CC!=names_.get()||
           source_->index4BC!=index_.get()||!names_||!index_||namesProduced_!=1472||indexProduced_!=1472*8||
           source_->child24||source_->text1C||source_->cacheCount438||source_->field4C8!=839)return false;
        ++sourcePortalNextSelection_;++sectionAttempts_;
        std::array<char,10> name{{'P','o','r','t','a','l',0,0,0,0}};
        if(index==9){name[6]='1';name[7]='0';}else name[6]=char('1'+index);
        auto lower=name;lower[0]='p';
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
        if(!found){sourcePC_=0x157DD8;return true;} // Actual0; caller bounds before first getter, never skips row.
        const auto& expected=sourcePortalSections_[index];if(mid!=expected.ordinal)return false;
        sectionReturned_=false;const auto length=std::strlen(name.data());
        std::memcpy(source_->name28.data(),name.data(),length);source_->name28[length]=0;sourcePC_=0x157E9C;
        const auto offset=little(index_.get()+std::size_t(mid)*8),bytes=little(index_.get()+std::size_t(mid)*8+4);
        blockSectionBytes_=bytes;blockPayloadReturned_=false; //Actual index length, not4C8 read size.
        if(offset!=expected.offset||bytes!=expected.bytes){sourcePC_=0x157EB0;return false;}
        // Existing839-byte source capacity suffices for all ten406..412-byte
        // sections. Every positive selection really seeks and rereads.
        sourcePC_=0x157EF4;
        if(!file_->seek12F640(*this,source_->file00,offset,d)){sourcePC_=0x157F08;return false;}
        std::array<std::uint8_t,4> count{};std::size_t done=0;
        if(!readExact(count.data(),4,done,0x157F28,0x157F30,d))return false;
        source_->selected20=little(count.data());sourcePC_=0x157F38;
        if(!readSourceSectionPayload157F4C(d))return false;
        if(source_->selected20!=expected.count)return false;
        sourcePortalSelected_=index;sourcePortalSelectedName_=name;
        sectionReturned_=true;returned=true;sourcePC_=0x15821C;return true;
    }
    bool readSourcePortalField158240(const char* lower,std::string_view& value,bool& found,
        std::uint32_t& reachedCopyPC,D& d)noexcept {
        d={};value={};found=false;reachedCopyPC=0;sourcePC_=0x158280;
        if(!sourcePortalConfigCurrent()||!lower)return false;
        bool admitted=false;
        static constexpr const char* keys[]={"vertex1","vertex2","vertex3","vertex4","connect1","connect2"};
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

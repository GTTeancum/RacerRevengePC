// Included inside same OriginalWorldSourceConfig50LA. Private RA Zone-only transport.
    struct SourceZoneSection {std::uint32_t ordinal,offset,bytes,count;};
    static constexpr std::array<SourceZoneSection,10> sourceZoneSections_{{
        {201u,123169u,338u,7u}, // zone1
        {735u,298571u,337u,7u}, // zone2
        {831u,330044u,197u,4u}, // zone3
        {865u,341342u,197u,4u}, // zone4
        {938u,365418u,197u,4u}, // zone5
        {962u,373328u,198u,4u}, // zone6
        {1210u,454748u,196u,4u}, // zone7
        {1244u,465897u,337u,7u}, // zone8
        {1367u,506162u,198u,4u}, // zone9
        {669u,276532u,196u,4u}, // zone10
    }};
    std::uint32_t sourceZoneNextSelection_=0,sourceZoneSelectedPass_=0;
    std::optional<std::uint32_t> sourceZoneSelected_;
    std::array<char,8> sourceZoneSelectedName_{}; // Native currentness only.
    bool sourceZoneConfigCurrent()const noexcept {
        if(!sourceZoneSelected_||*sourceZoneSelected_>=10||sourceZoneSelectedPass_>1)return false;
        const auto& expected=sourceZoneSections_[*sourceZoneSelected_];
        return thread_==std::this_thread::get_id()&&source_&&!sourceClosed_&&!nativeClosed_&&sectionReturned_&&
            sourceZoneNextSelection_&&sectionAttempts_==190u+sourceZoneNextSelection_&&
            source_->binary08==1&&source_->opened0C==1&&source_->mode10==0&&
            source_->selected20==expected.count&&source_->field4C8==839&&
            source_->block4C4==block_.get()&&block_&&sourceSectionPayloadCurrent(expected.bytes)&&!lostBlock_&&
            std::strcmp(source_->name28.data(),sourceZoneSelectedName_.data())==0&&
            file_&&file_->current(*this,source_->file00);
    }
    bool selectSourceZone157D00(std::uint32_t index,std::uint32_t pass,bool& returned,D& d)noexcept {
        d={};returned=false;sourcePC_=0x157D1C;
        if(index>=10||pass>1||sourceZoneNextSelection_!=index*2u+pass||
           sectionAttempts_!=190u+sourceZoneNextSelection_)return false;
        if(sourceZoneNextSelection_==0){
            // Genuine Fog0 leaves the Light185 selection unchanged. Do not
            // hide its later NULL3AC dereference behind an invented default.
            if(!sourceFogSelectionAttempted_||
               !(sourceFogConfigCurrent()||(!sourceFogSelectionReturned_&&sourceLightConfigCurrent())))return false;
        }else if(!sourceZoneConfigCurrent())return false;
        if(returned_!=Return::one||source_->count14!=1472||source_->names4CC!=names_.get()||
           source_->index4BC!=index_.get()||!names_||!index_||namesProduced_!=1472||indexProduced_!=1472*8||
           source_->child24||source_->text1C||source_->cacheCount438||source_->field4C8!=839)return false;
        ++sourceZoneNextSelection_;++sectionAttempts_;
        std::array<char,8> name{{'Z','o','n','e',0,0,0,0}};
        if(index==9){name[4]='1';name[5]='0';}else name[4]=char('1'+index);
        auto lower=name;lower[0]='z';
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
        if(!found){sourcePC_=0x157DD8;return true;} // Caller bounds before unjoined fallback/log, no fabricated row return.
        const auto& expected=sourceZoneSections_[index];if(mid!=expected.ordinal)return false;
        sectionReturned_=false;const auto length=std::strlen(name.data());
        std::memcpy(source_->name28.data(),name.data(),length);source_->name28[length]=0;sourcePC_=0x157E9C;
        const auto offset=little(index_.get()+std::size_t(mid)*8),bytes=little(index_.get()+std::size_t(mid)*8+4);
        blockSectionBytes_=bytes;blockPayloadReturned_=false; //Actual index length, not4C8 read size.
        if(offset!=expected.offset||bytes!=expected.bytes){sourcePC_=0x157EB0;return false;}
        // Existing839-byte source capacity suffices for all ten196..338byte
        // sections. Second pass still really seeks and rereads SAME section.
        sourcePC_=0x157EF4;
        if(!file_->seek12F640(*this,source_->file00,offset,d)){sourcePC_=0x157F08;return false;}
        std::array<std::uint8_t,4> count{};std::size_t done=0;
        if(!readExact(count.data(),4,done,0x157F28,0x157F30,d))return false;
        source_->selected20=little(count.data());sourcePC_=0x157F38;
        if(!readSourceSectionPayload157F4C(d))return false;
        if(source_->selected20!=expected.count)return false;
        sourceZoneSelected_=index;sourceZoneSelectedPass_=pass;sourceZoneSelectedName_=name;
        sectionReturned_=true;returned=true;sourcePC_=0x15821C;return true;
    }
    bool readSourceZoneField158240(const char* lower,std::string_view& value,bool& found,
        std::uint32_t& reachedCopyPC,D& d)noexcept {
        d={};value={};found=false;reachedCopyPC=0;sourcePC_=0x158280;
        if(!sourceZoneConfigCurrent()||!lower)return false;
        bool admitted=false;
        if(sourceZoneSelectedPass_==1)admitted=std::strcmp(lower,"numberofcollisionobjects")==0;
        else{
            static constexpr const char* keys[]={"environmentnodevisible","numberofstaticmodels","searchtype",
                "xmin","xmax","zmin","zmax","zonereverb","fogcolorrgb","fogvisibility","foghaziness","exclusionlist"};
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

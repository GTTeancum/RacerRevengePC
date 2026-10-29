// Private actual AUB collision/drive descriptor selections. Tuples qualify real File reads.
    struct SourceMaterialSoundSection {const char* name;const char* lower;std::uint32_t ordinal,offset,bytes,count,capacity;};
    inline static constexpr SourceMaterialSoundSection sourceMaterialSoundSections_[]={
        {"Sounds","sounds",73,22279,228,5,451},
        {"MaterialCollisionSound1","materialcollisionsound1",7,2666,61,1,451},
        {"MaterialCollisionSound2","materialcollisionsound2",11,2926,61,1,451},
        {"MaterialCollisionSound3","materialcollisionsound3",12,2991,61,1,451},
        {"MaterialCollisionSound4","materialcollisionsound4",13,3056,61,1,451},
        {"MaterialCollisionSound5","materialcollisionsound5",14,3121,61,1,451},
        {"MaterialCollisionSound6","materialcollisionsound6",15,3186,61,1,451},
        {"MaterialCollisionSound7","materialcollisionsound7",16,3251,61,1,451},
        {"MaterialCollisionSound8","materialcollisionsound8",17,3316,61,1,451},
        {"MaterialCollisionSound9","materialcollisionsound9",18,3381,61,1,451},
        {"MaterialCollisionSound10","materialcollisionsound10",8,2731,61,1,451},
        {"MaterialCollisionSound11","materialcollisionsound11",9,2796,61,1,451},
        {"MaterialCollisionSound12","materialcollisionsound12",10,2861,61,1,451},
        {"Sounds","sounds",73,22279,228,5,451},
        {"MaterialDriveOnSound1","materialdriveonsound1",19,3446,339,7,451},
        {"MaterialDriveOnSound2","materialdriveonsound2",23,4818,339,7,451},
        {"MaterialDriveOnSound3","materialdriveonsound3",24,5161,339,7,451},
        {"MaterialDriveOnSound4","materialdriveonsound4",25,5504,339,7,451},
        {"MaterialDriveOnSound5","materialdriveonsound5",26,5847,339,7,451},
        {"MaterialDriveOnSound6","materialdriveonsound6",27,6190,339,7,451},
        {"MaterialDriveOnSound7","materialdriveonsound7",28,6533,339,7,451},
        {"MaterialDriveOnSound8","materialdriveonsound8",29,6876,339,7,451},
        {"MaterialDriveOnSound9","materialdriveonsound9",30,7219,339,7,451},
        {"MaterialDriveOnSound10","materialdriveonsound10",20,3789,339,7,451},
        {"MaterialDriveOnSound11","materialdriveonsound11",21,4132,339,7,451},
        {"MaterialDriveOnSound12","materialdriveonsound12",22,4475,339,7,451},
    };
    std::uint32_t sourceMaterialSoundNextSelection_=0,sourceMaterialSoundBaseAttempt_=0;
    std::optional<std::uint32_t> sourceMaterialSoundSelected_;
    bool sourceMaterialSoundDirectoryCurrent()const noexcept {
        return thread_==std::this_thread::get_id()&&cohort_==Cohort::audioAub&&
            allocated_&&!sourceClosed_&&!nativeClosed_&&source_&&file_&&opens_==1&&
            attemptRetained_&&returned_==Return::one&&file_->current(*this,source_->file00)&&
            source_->binary08==1&&source_->opened0C==1&&source_->mode10==0&&
            source_->count14==74&&names_&&index_&&source_->names4CC==names_.get()&&
            source_->index4BC==index_.get()&&source_->indexBytes4C0==74u*8u&&
            namesProduced_==74&&indexProduced_==74u*8u&&!source_->text1C&&
            !source_->child24&&!source_->cacheCount438;
    }
    bool sourceMaterialSoundConfigCurrent()const noexcept {
        if(!sourceMaterialSoundSelected_||*sourceMaterialSoundSelected_>=26)return false;
        const auto& expected=sourceMaterialSoundSections_[*sourceMaterialSoundSelected_];
        return sourceMaterialSoundDirectoryCurrent()&&sectionReturned_&&sourceMaterialSoundNextSelection_&&
            *sourceMaterialSoundSelected_+1u==sourceMaterialSoundNextSelection_&&
            sectionAttempts_==sourceMaterialSoundBaseAttempt_+sourceMaterialSoundNextSelection_&&
            source_->selected20==expected.count&&source_->field4C8==expected.capacity&&
            source_->block4C4==block_.get()&&block_&&sourceSectionPayloadCurrent(expected.bytes)&&
            std::strcmp(source_->name28.data(),expected.name)==0;
    }
    bool selectSourceMaterialSoundSection157D00(std::uint32_t selected,bool& returned,D& d)noexcept {
        d={};returned=false;sourcePC_=0x157D1C;
        if(selected>=26||selected!=sourceMaterialSoundNextSelection_)return false;
        if(!selected){
            if(sourceSoulTreeSoundNextSelection_!=43||!sourceSoulTreeSoundSelected_||*sourceSoulTreeSoundSelected_!=42||!sourceSoulTreeSoundConfigCurrent())return false;
            sourceMaterialSoundBaseAttempt_=sectionAttempts_;
        }else if(!sourceMaterialSoundConfigCurrent())return false;
        if(!sourceMaterialSoundDirectoryCurrent()||sectionAttempts_!=sourceMaterialSoundBaseAttempt_+selected)return false;
        ++sourceMaterialSoundNextSelection_;++sectionAttempts_;
        const auto& expected=sourceMaterialSoundSections_[selected];
        const auto compare=[&](std::uint32_t i,int& result)noexcept {
            if(i>=74)return false;const auto* candidate=reinterpret_cast<const char*>(names_.get()+std::size_t(i)*1024);
            if(!std::memchr(candidate,0,1024))return false;result=std::strcmp(expected.lower,candidate);return true;
        };
        std::uint32_t low=0,high=73,mid=high/2;int compared=0;bool found=false;
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
        //Actual157F48 loads a3 from4C8, not selected index byte size. Preserve
        //trailing bytes read into the same retained cache after a smaller section.
        if(!readSourceSectionPayload157F4C(d))return false;
        if(source_->selected20!=expected.count)return false;
        sourceMaterialSoundSelected_=selected;
        sectionReturned_=true;returned=true;sourcePC_=0x15821C;return true;
    }
    bool readSourceMaterialSoundField158240(const char* lower,std::string_view& value,bool& found,
        std::uint32_t& reachedCopyPC,D& d)noexcept {
        d={};value={};found=false;reachedCopyPC=0;sourcePC_=0x158280;
        if(!sourceMaterialSoundConfigCurrent()||!lower)return false;
        bool admitted=false;
        if(!*sourceMaterialSoundSelected_)admitted=std::strcmp(lower,"nummaterialcollisionsounds")==0;
        else if(*sourceMaterialSoundSelected_==13)admitted=std::strcmp(lower,"nummaterialdriveonsounds")==0;
        else {
            static constexpr const char* keys[]={"soundresourcefile","position","looping","oneshot",
                "resetwheninaudible","force2d","stereo","oneshotdistance","randomtriggerpercent",
                "maxdistance","mindistance","priority"};
            for(const auto* key:keys)if(std::strcmp(lower,key)==0){admitted=true;break;}
            if(*sourceMaterialSoundSelected_>=14){
                static constexpr const char* driveKeys[]={"materialminspeed","materialmaxspeed","materialsamplespeed",
                    "materialresonantfreqspeed","materialminvolume","materialfreqslope"};
                for(const auto* key:driveKeys)if(std::strcmp(lower,key)==0){admitted=true;break;}
            }
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

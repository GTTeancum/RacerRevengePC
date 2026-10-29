// Private actual AUB Sounds reread then SoulTreeSound1..42. Tuples qualify real File reads.
    struct SourceSoulTreeSoundSection {const char* name;const char* lower;std::uint32_t ordinal,offset,bytes,count,capacity;};
    inline static constexpr SourceSoulTreeSoundSection sourceSoulTreeSoundSections_[]={
        {"Sounds","sounds",73,22279,228,5,228},
        {"SoulTreeSound1","soultreesound1",31,7562,313,6,313},
        {"SoulTreeSound2","soultreesound2",42,11002,313,6,313},
        {"SoulTreeSound3","soultreesound3",53,14870,313,6,313},
        {"SoulTreeSound4","soultreesound4",64,19300,313,6,313},
        {"SoulTreeSound5","soultreesound5",68,20702,313,6,313},
        {"SoulTreeSound6","soultreesound6",69,21019,313,6,313},
        {"SoulTreeSound7","soultreesound7",70,21336,313,6,313},
        {"SoulTreeSound8","soultreesound8",71,21653,309,6,313},
        {"SoulTreeSound9","soultreesound9",72,21966,309,6,313},
        {"SoulTreeSound10","soultreesound10",32,7879,309,6,313},
        {"SoulTreeSound11","soultreesound11",33,8192,309,6,313},
        {"SoulTreeSound12","soultreesound12",34,8505,307,6,313},
        {"SoulTreeSound13","soultreesound13",35,8816,310,6,313},
        {"SoulTreeSound14","soultreesound14",36,9130,307,6,313},
        {"SoulTreeSound15","soultreesound15",37,9441,307,6,313},
        {"SoulTreeSound16","soultreesound16",38,9752,307,6,313},
        {"SoulTreeSound17","soultreesound17",39,10063,307,6,313},
        {"SoulTreeSound18","soultreesound18",40,10374,310,6,313},
        {"SoulTreeSound19","soultreesound19",41,10688,310,6,313},
        {"SoulTreeSound20","soultreesound20",43,11319,307,6,313},
        {"SoulTreeSound21","soultreesound21",44,11630,310,6,313},
        {"SoulTreeSound22","soultreesound22",45,11944,310,6,313},
        {"SoulTreeSound23","soultreesound23",46,12258,310,6,313},
        {"SoulTreeSound24","soultreesound24",47,12572,311,6,313},
        {"SoulTreeSound25","soultreesound25",48,12887,312,6,313},
        {"SoulTreeSound26","soultreesound26",49,13203,312,6,313},
        {"SoulTreeSound27","soultreesound27",50,13519,447,9,447},
        {"SoulTreeSound28","soultreesound28",51,13970,446,9,447},
        {"SoulTreeSound29","soultreesound29",52,14420,446,9,447},
        {"SoulTreeSound30","soultreesound30",54,15187,447,9,447},
        {"SoulTreeSound31","soultreesound31",55,15638,447,9,447},
        {"SoulTreeSound32","soultreesound32",56,16089,448,9,448},
        {"SoulTreeSound33","soultreesound33",57,16541,451,9,451},
        {"SoulTreeSound34","soultreesound34",58,16996,450,9,451},
        {"SoulTreeSound35","soultreesound35",59,17450,450,9,451},
        {"SoulTreeSound36","soultreesound36",60,17904,310,6,451},
        {"SoulTreeSound37","soultreesound37",61,18218,450,9,451},
        {"SoulTreeSound38","soultreesound38",62,18672,312,6,451},
        {"SoulTreeSound39","soultreesound39",63,18988,308,6,451},
        {"SoulTreeSound40","soultreesound40",65,19617,308,6,451},
        {"SoulTreeSound41","soultreesound41",66,19929,314,6,451},
        {"SoulTreeSound42","soultreesound42",67,20247,451,9,451},
    };
    std::uint32_t sourceSoulTreeSoundNextSelection_=0,sourceSoulTreeSoundBaseAttempt_=0;
    std::optional<std::uint32_t> sourceSoulTreeSoundSelected_;
    bool sourceSoulTreeSoundDirectoryCurrent()const noexcept {
        return thread_==std::this_thread::get_id()&&cohort_==Cohort::audioAub&&
            allocated_&&!sourceClosed_&&!nativeClosed_&&source_&&file_&&opens_==1&&
            attemptRetained_&&returned_==Return::one&&file_->current(*this,source_->file00)&&
            source_->binary08==1&&source_->opened0C==1&&source_->mode10==0&&
            source_->count14==74&&names_&&index_&&source_->names4CC==names_.get()&&
            source_->index4BC==index_.get()&&source_->indexBytes4C0==74u*8u&&
            namesProduced_==74&&indexProduced_==74u*8u&&!source_->text1C&&
            !source_->child24&&!source_->cacheCount438;
    }
    bool sourceSoulTreeSoundConfigCurrent()const noexcept {
        if(!sourceSoulTreeSoundSelected_||*sourceSoulTreeSoundSelected_>=43)return false;
        const auto& expected=sourceSoulTreeSoundSections_[*sourceSoulTreeSoundSelected_];
        return sourceSoulTreeSoundDirectoryCurrent()&&sectionReturned_&&sourceSoulTreeSoundNextSelection_&&
            *sourceSoulTreeSoundSelected_+1u==sourceSoulTreeSoundNextSelection_&&
            sectionAttempts_==sourceSoulTreeSoundBaseAttempt_+sourceSoulTreeSoundNextSelection_&&
            source_->selected20==expected.count&&source_->field4C8==expected.capacity&&
            source_->block4C4==block_.get()&&block_&&sourceSectionPayloadCurrent(expected.bytes)&&
            std::strcmp(source_->name28.data(),expected.name)==0;
    }
    bool selectSourceSoulTreeSoundSection157D00(std::uint32_t selected,bool& returned,D& d)noexcept {
        d={};returned=false;sourcePC_=0x157D1C;
        if(selected>=43||selected!=sourceSoulTreeSoundNextSelection_)return false;
        if(!selected){
            if(!sourceAmbientCountReturned_||!sourceAmbientSoundsConfigCurrent())return false;
            sourceSoulTreeSoundBaseAttempt_=sectionAttempts_;
        }else if(!sourceSoulTreeSoundConfigCurrent())return false;
        if(!sourceSoulTreeSoundDirectoryCurrent()||sectionAttempts_!=sourceSoulTreeSoundBaseAttempt_+selected)return false;
        ++sourceSoulTreeSoundNextSelection_;++sectionAttempts_;
        const auto& expected=sourceSoulTreeSoundSections_[selected];
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
        sourceSoulTreeSoundSelected_=selected;
        sectionReturned_=true;returned=true;sourcePC_=0x15821C;return true;
    }
    bool readSourceSoulTreeSoundField158240(const char* lower,std::string_view& value,bool& found,
        std::uint32_t& reachedCopyPC,D& d)noexcept {
        d={};value={};found=false;reachedCopyPC=0;sourcePC_=0x158280;
        if(!sourceSoulTreeSoundConfigCurrent()||!lower)return false;
        bool admitted=false;
        if(!*sourceSoulTreeSoundSelected_)admitted=std::strcmp(lower,"numsoultreesounds")==0;
        else {
            static constexpr const char* keys[]={"soundresourcefile","position","looping","oneshot",
                "resetwheninaudible","force2d","stereo","oneshotdistance","randomtriggerpercent",
                "maxdistance","mindistance","priority","attachtoobject"};
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

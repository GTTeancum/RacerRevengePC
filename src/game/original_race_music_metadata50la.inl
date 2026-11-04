// Private continuation of the returned first binary open. All bytes below
// originate in the retained numeric FileManager occurrence, not a JUB parser.
// Each playlist index requires the SAME backend's genuine returned Stream load.
bool OriginalRaceMusicManager50LA::loadSourceMusicMetadata2C0BE4(
    const ::original_main_entry50KG::Application& app,OriginalRaceConstructor50LA& race,
    OriginalRaceMusicManagerDiagnostic50LA& d)noexcept {
    if(!impl_||!impl_->sourceOnly||!impl_->sourceChild)
        return Impl::stop(d,0x2c0be4,"Actual original music child absent");
    auto& p=*impl_;auto& child=*p.sourceChild;
    const auto stop=[&](std::uint32_t pc,const char* why)noexcept {
        p.sourcePC=child.pc=pc;return Impl::stop(d,pc,why);};
    if(!p.componentCurrent()||race.music10C_.get()!=this||race.application_!=p.application||
       race.application_!=app.entities()||child.pc!=0x2c0be4||child.metadataAttempted||
       !child.config60||child.configOpenReturn!=1u||!child.configFile)
        return stop(0x2c0be4,"Returned original Config occurrence unavailable");
    auto& config=*child.config60;auto& fd=child.configFileDiagnostic;
    if(!child.configFile->current(&config,config.file00))
        return stop(0x157d00,"Original Config file lifetime unavailable");
    child.metadataAttempted=true;
    const auto le=[](const std::uint8_t* b)noexcept {
        return std::uint32_t(b[0])|(std::uint32_t(b[1])<<8u)|
            (std::uint32_t(b[2])<<16u)|(std::uint32_t(b[3])<<24u);};
    const auto lower=[](char c)noexcept {return c>='A'&&c<='Z'?char(c+('a'-'A')):c;};
    const auto key=[&](auto& lookup,std::string_view requested,std::uint32_t pc)noexcept {
        if(requested.size()>=lookup.lowerKey.size())return stop(pc,"Original lookup strcpy exceeds native extent");
        for(std::size_t i=0;i<requested.size();++i){
            if(static_cast<unsigned char>(requested[i])>127u)return stop(pc,"Non-ASCII original lowercase path is not joined");
            lookup.lowerKey[i]=lower(requested[i]);
        }
        lookup.lowerKey[requested.size()]=0;lookup.lowerKnown=requested.size()+1;return true;
    };
    // Actual midpoint/low/high algorithm, including the terminal duplicate
    // low comparison. Each comparator consumes only reached known bytes.
    const auto find=[&](auto& lookup,std::uint32_t count,auto&& compare,const std::array<std::uint32_t,3>& pcs,bool single)noexcept {
        const auto pc=pcs[0];
        if(!count||count>48u)return stop(pc,"Original binary search index is outside this retained cohort");
        const auto visit=[&](std::int32_t index,int& cmp,std::uint8_t arm)noexcept {
            lookup.comparisonPC=pcs[arm?arm-1u:0u];
            if(lookup.visitedCount==lookup.visited.size())return stop(lookup.comparisonPC,"Native comparison receipt capacity exceeded");
            lookup.visited[lookup.visitedCount++]=index;return compare(index,cmp);
        };
        std::int32_t low=0,high=static_cast<std::int32_t>(count-1u),mid=high>>1;int cmp=0;
        if(single){if(!visit(0,cmp,0))return false;lookup.result=cmp? -1:0;lookup.matchArm=0;return true;}
        for(;;){
            if(!visit(mid,cmp,1))return false;
            if(!cmp){lookup.result=mid;lookup.matchArm=1;return true;}
            if(cmp<0)high=mid;else low=mid;
            const auto gap=high-low;mid=low+(gap>>1);if(gap<2)break;
        }
        if(!visit(low,cmp,2))return false;
        if(!cmp){lookup.result=low;lookup.matchArm=2;return true;}
        if(!visit(high,cmp,3))return false;
        lookup.result=cmp? -1:high;if(!cmp)lookup.matchArm=3;return true;
    };
    const auto read=[&](std::uint32_t pc,void* target,std::size_t bytes,std::size_t& copied)noexcept {
        if(child.configReadCount==child.configReads.size())return stop(pc,"Native Config read receipt capacity exceeded");
        auto& receipt=child.configReads[child.configReadCount++];receipt.callPC=pc;receipt.requested=bytes;
        receipt.returned=child.configFile->read12FB30(&config,config.file00,target,bytes,receipt.copied,fd);
        copied=receipt.copied;
        if(!receipt.returned)return stop(fd.sourcePC,"Original selected Config read remains bounded");
        return true;
    };
    const auto select=[&](std::size_t ordinal,std::string_view requested)noexcept {
        auto& state=child.selections[ordinal];auto& lookup=state.lookup;
        if(!config.opened0C)return stop(0x157d2c,"Original unopened Config assertion path is not joined");
        if(config.mode10)return stop(0x157db4,"Original writable Config flush path is not joined");
        if(config.binary08!=1u)return stop(0x157b10,"Original nonbinary selection is not joined");
        if(!config.names4CC){lookup.result=-1;state.returned=false;return true;} //157B24/157DD8.
        if(config.names4CC!=child.names4CC.get()||config.count14!=48u)
            return stop(0x157b60,"Original section-name ownership changed");
        if(!key(lookup,requested,0x157b30))return false;
        const auto compare=[&](std::int32_t index,int& cmp)noexcept {
            if(index<0||std::size_t(index)>=child.nameKnown.size())return stop(lookup.comparisonPC,"Original section index is not represented");
            const auto* name=child.names4CC.get()+std::size_t(index)*1024u;
            for(std::size_t at=0;at<lookup.lowerKnown;++at){
                if(at>=child.nameKnown[index])return stop(lookup.comparisonPC,"Original section strcmp would consume unknown bytes");
                const auto a=static_cast<unsigned char>(lookup.lowerKey[at]);const auto b=name[at];
                if(a!=b){cmp=a<b?-1:1;return true;}if(!a){cmp=0;return true;}
            }
            return stop(lookup.comparisonPC,"Original section strcmp extent unavailable");
        };
        if(!find(lookup,config.count14,compare,{0x157b8c,0x157be8,0x157c1c},false))return false;
        if(*lookup.result<0){state.returned=false;return true;} // Existing selection is not cleared.
        const auto length=std::min<std::size_t>(requested.size(),1023u);
        std::memcpy(config.name28.data(),requested.data(),length);config.name28[length]=0;child.configNameKnown=length+1;
        const auto index=std::size_t(*lookup.result)*8u;
        if(config.index4BC!=child.index4BC.get()||index+4u>child.indexKnown)
            return stop(0x157eb0,"Original section offset would consume unknown bytes");
        state.offset=le(child.index4BC.get()+index);
        if(index+8u>child.indexKnown)return stop(0x157eb8,"Original section length would consume unknown bytes");
        state.length=le(child.index4BC.get()+index+4u);
        if(!*state.length||*state.length>1024u)return stop(config.block4C4?0x157ebc:0x157ee4,
            "Section payload exceeds native cohort; no capacity store reached");
        if(!config.block4C4||config.field4C8<*state.length){
            const bool replacing=config.block4C4!=nullptr;
            if(replacing&&(config.block4C4!=child.block4C4.get()||!child.blockKnown))
                return stop(0x157ecc,"Original selected-block ownership changed");
            config.field4C8=*state.length; //157ECC or157EE4, BEFORE allocator call.
            std::unique_ptr<std::uint8_t[]> next,known;
            try {next.reset(new std::uint8_t[config.field4C8]);known.reset(new std::uint8_t[config.field4C8]);}
            catch(...){return stop(replacing?0x157ed4:0x157ee8,"Native block backing unavailable; source allocation return unknown");}
            std::fill_n(known.get(),config.field4C8,std::uint8_t(0)); // Native knownness only; payload is NOT cleared.
            if(replacing){const auto kept=std::min<std::size_t>(child.blockCapacity,config.field4C8);
                // Only copy initialized payload bytes; source unknown bytes stay
                // unknown instead of being read as C++ indeterminate values.
                for(std::size_t n=0;n<kept;++n)if(child.blockKnown[n]){next[n]=child.block4C4[n];known[n]=1;}}
            child.block4C4=std::move(next);child.blockKnown=std::move(known);
            child.blockCapacity=config.field4C8;config.block4C4=child.block4C4.get(); //157EE0/157EF0.
        }
        if(config.block4C4!=child.block4C4.get()||!child.blockKnown||child.blockCapacity<config.field4C8)
            return stop(0x157ef4,"Original selected block native extent unavailable");
        if(!child.configFile->seekSet12F640(&config,config.file00,*state.offset,fd))
            return stop(fd.sourcePC,"Original Config section seek remains bounded");
        state.seekReturned=true;state.fieldCount86C.fill(0);state.countZeroed=true; //157F10.
        std::size_t copied=0;if(!read(0x157f28,state.fieldCount86C.data(),4,copied))return false;
        config.selected20=le(state.fieldCount86C.data()); //157F38, including genuine zero suffix on short read.
        const bool returned=read(0x157f4c,child.block4C4.get(),config.field4C8,copied);
        for(std::size_t n=0;n<copied;++n)child.blockKnown[n]=1; // Keep old known suffix if read was short.
        if(!returned)return false;
        state.returned=true;return true; //157F58 ignores the actual read byte count.
    };
    const auto getString=[&](auto& state,std::string_view requested,char* output,std::size_t extent,std::int32_t limit)noexcept {
        if(config.binary08!=1u)return stop(0x158278,"Original text getter path is not joined");
        if(!config.selected20){output[0]=0;state.produced=1;state.returned=false;return true;} //15828C.
        const bool single=config.selected20==1u;
        if(!key(state.lookup,requested,single?0x1582ac:0x15837c))return false;
        if(!config.block4C4||config.block4C4!=child.block4C4.get()||!child.blockKnown)
            return stop(single?0x1582dc:0x1583e8,"Original field block unavailable");
        const auto compare=[&](std::int32_t index,int& cmp)noexcept {
            if(index<0)return stop(state.lookup.comparisonPC,"Original field index unavailable");
            const auto base=std::size_t(index)*43u;
            for(std::size_t n=0;n<state.lookup.lowerKnown;++n){
                if(base+n>=child.blockCapacity||!child.blockKnown[base+n])
                    return stop(state.lookup.comparisonPC,"Original field strcmp would consume unknown bytes");
                const auto a=static_cast<unsigned char>(state.lookup.lowerKey[n]),b=child.block4C4[base+n];
                if(a!=b){cmp=a<b?-1:1;return true;}if(!a){cmp=0;return true;}
            }
            return stop(state.lookup.comparisonPC,"Original field strcmp extent unavailable");
        };
        if(!find(state.lookup,config.selected20,compare,{single?0x1582e4u:0x1583f8u,0x1584f0,0x1585b0},single))return false;
        if(*state.lookup.result<0){state.returned=false;return true;} // A nonempty-section miss leaves output untouched.
        constexpr std::array<std::uint32_t,4> offsetPC{0x158300,0x158410,0x15850c,0x1585cc};
        constexpr std::array<std::uint32_t,4> strlenPC{0x158314,0x158424,0x158520,0x1585e0};
        constexpr std::array<std::uint32_t,4> limitPC{0x15832c,0x15843c,0x158538,0x1585f8};
        constexpr std::array<std::uint32_t,4> copyPC{0x158354,0x158474,0x158560,0x158620};
        const auto arm=state.lookup.matchArm;
        const auto offsetAt=std::size_t(*state.lookup.result)*43u+35u;
        if(offsetAt+4u>child.blockCapacity)return stop(offsetPC[arm],"Original unaligned value offset exceeds block extent");
        for(std::size_t n=0;n<4;++n)if(!child.blockKnown[offsetAt+n])
            return stop(offsetPC[arm],"Original value offset would consume unknown bytes");
        const auto valueAt=std::size_t(le(child.block4C4.get()+offsetAt));std::size_t bytes=0;
        for(;;++bytes){
            if(valueAt>child.blockCapacity||bytes>=child.blockCapacity-valueAt||!child.blockKnown[valueAt+bytes])
                return stop(strlenPC[arm],"Original value strlen would consume unknown/out-of-extent bytes");
            if(!child.block4C4[valueAt+bytes])break;
        }
        if(limit!=-1){if(limit<=0)return stop(limitPC[arm],"Original getter limit is outside the admitted positive cohort");
            bytes=std::min<std::size_t>(bytes,std::size_t(limit-1));}
        if(bytes>=extent)return stop(copyPC[arm],"Original getter copy exceeds native destination extent");
        std::memcpy(output,child.block4C4.get()+valueAt,bytes);output[bytes]=0;
        state.produced=bytes+1;state.returned=true;return true;
    };
    if(!select(0,"Songs"))return false; //2C0BEC ignores the returned source boolean.
    if(!getString(child.numberGetter,"NumberOfSongs",child.numberStack20.data(),child.numberStack20.size(),-1))return false;
    if(!*child.numberGetter.returned){
        child.count34=0;child.numberOfSongsReturned=false; //158B60 default0.
        return stop(0x2c0d90,"Original missing song count reached its unimplemented assertion path");
    }
    const auto number=std::string_view(child.numberStack20.data(),child.numberGetter.produced-1u);
    std::uint32_t converted=0;
    if(!number.empty()&&lower(number[0])=='t')converted=1;
    else if(!number.empty()&&lower(number[0])=='f')converted=0;
    else if(number.size()==2u&&lower(number[0])=='o'&&lower(number[1])=='n')converted=1;
    else if(number.size()==3u&&lower(number[0])=='o'&&lower(number[1])=='f'&&lower(number[2])=='f')converted=0;
    else if(!number.empty()&&lower(number[0])=='y')converted=1;
    else if(!number.empty()&&lower(number[0])=='n')converted=0;
    else {
        // Exact admitted positive decimal strtol(base0) subset, including low32
        // truncation. Other whitespace/sign/base/overflow cases remain
        // bounded; they are not strict-parse source failures or guessed zero.
        using G=assets::OriginalConfigProcessGlobals50LA;
        const G::MusicSprintfAccess access;std::uint32_t conversionPC=0x30ae84;
        if(!G::readMusicIntegerReent30AE84(access,conversionPC))
            return stop(conversionPC,"Original song-count integer reentrancy read remains bounded");
        if(number.empty()||number.size()>10u||number[0]=='0')return stop(0x158a34,"Original integer conversion outside admitted decimal subset");
        std::uint64_t value=0;
        for(const char c:number){if(c<'0'||c>'9')return stop(0x158a34,"Original integer conversion outside admitted decimal subset");value=value*10u+std::uint32_t(c-'0');}
        converted=static_cast<std::uint32_t>(value);
    }
    child.count34=converted;child.numberOfSongsReturned=true; //158A44,158B58 ->2C0C0C.
    if(converted!=47u)return stop(0x2c0c20,"Original song count outside this authenticated playlist cohort");
    child.songEntryBytes=std::size_t(converted)*0x108u;
    try {child.songEntries58.reset(new std::uint8_t[child.songEntryBytes]);}
    catch(...){return stop(0x2c0c20,"Native playlist backing unavailable; source malloc return unknown");}
    child.songOrdinal=1;child.sourceSongs58=child.songEntries58.get();child.songs58Published=true;
    child.word58.reset(); // Typed pointer is known; never encode a host address as a guest word.
    while(*child.songOrdinal<=converted){
        const auto ordinal=*child.songOrdinal;
        auto& song=child.songLookups[ordinal-1u];
        auto& filenameGetter=child.filenameGetters[ordinal-1u];
        child.songContinueS3=1; //2C0C40 BEFORE formatting and section selection.
        child.pc=p.sourcePC=0x2c0c44;
        if(!formatSourceSong2C0C44(ordinal,d)){p.sourcePC=child.pc;return false;}
        if(!select(ordinal,std::string_view(child.songSection860.data(),child.songSectionKnown-1u)))return false;
        if(*child.selections[ordinal].returned){
            if(!getString(filenameGetter,"Filename",child.songFilename530.data(),child.songFilename530.size(),0x104))return false;
            if(filenameGetter.produced)child.songFilenameKnown=filenameGetter.produced;
            child.songContinueS3=*filenameGetter.returned?1u:0u;
        }
        //A section miss leaves s3=1 and the SAME prior stack530 bytes. It is
        //not a convenient false return. On the first iteration they are unknown.
        if(!*child.songContinueS3)return stop(0x2c0cf0,"Original missing filename assertion path is not joined");
        if(!child.songFilenameKnown)return stop(0x2c0cb8,"Original filename strcpy would consume unknown stack bytes");
        std::size_t nameBytes=0;
        while(nameBytes<child.songFilenameKnown&&child.songFilename530[nameBytes])++nameBytes;
        if(nameBytes==child.songFilenameKnown)return stop(0x2c0cb8,"Original filename strcpy has no known terminator");
        ++nameBytes;
        const auto offset=std::size_t(ordinal-1u)*0x108u;
        if(child.sourceSongs58!=child.songEntries58.get()||!child.songs58Published||
           offset>child.songEntryBytes||0x108u>child.songEntryBytes-offset||nameBytes>0x104u)
            return stop(0x2c0cb8,"Original playlist entry allocation identity or extent changed");
        auto* const entry=child.songEntries58.get()+offset;
        std::memcpy(entry,child.songFilename530.data(),nameBytes); //Actual309230 at2C0CB8.
        song.filenameKnown=nameBytes;song.copied=true; //No name-tail or index initialization.
        child.pc=p.sourcePC=0x2c0ce0;
        //The concrete manager derives both arguments from its SAME live264-byte
        //entry; neither is a public caller-selected buffer or metadata index.
        if(!p.componentCurrent()||!p.aural||!p.aural->current()||
           p.aural->issuer_!=&app||p.aural->entities_!=p.application||
           !p.aural->interfaceA0_||!*p.aural->interfaceA0_||
           child.backend3C!=p.nativeInterface44||
           p.nativeInterface44!=&**p.aural->interfaceA0_||
           race.music10C_.get()!=this||race.application_!=p.application)
            return stop(0x2c0ce0,"Actual music caller and same-Aural interface identity unavailable");
        auto& library=(**p.aural->interfaceA0_).sourceStreamLibrary;
        platform::OriginalStreamLibrarySource50LA::Diagnostic lookupDiagnostic;
        if(!library.lookup2684E0(song.occurrence,reinterpret_cast<char*>(entry),
                song.filenameKnown,0x104u,entry+0x104u,song.index,lookupDiagnostic))
            return stop(lookupDiagnostic.sourcePC,lookupDiagnostic.reason);
        song.sourceReturn=1u;++child.lookupReturns; //Actual2684E0 return; caller ignores its value.
        ++*child.songOrdinal;child.pc=p.sourcePC=0x2c0cec; //Only after actual lookup return.
        //2C0D78 signed count<s0; admitted count47 and ordinal1..48 avoid overflow.
    }
    child.pc=p.sourcePC=0x2c0e08;
    return stop(0x2c0e08,"Original47-song loop returned its lookups; automatic Config destruction remains unissued");
}

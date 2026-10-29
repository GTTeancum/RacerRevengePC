// Included within OriginalWorldSourceConfig50LA. No host strtok/locale parsing.
Text::Bytes textFieldScratch_,textFieldKey_;
std::optional<std::size_t> textTokenNext_;
std::size_t textTokenCurrent_=0;
bool textKnownLength(const Text::Bytes& bytes,std::size_t start,std::size_t& length,std::uint32_t pc)noexcept {
    sourcePC_=pc;length=0;
    for(std::size_t i=start;i<1024;++i){
        if(!bytes.known[i])return false;
        if(!bytes.value[i]){length=i-start;return true;}
    }
    return false;
}
//29FFA0 mutates its SAME scratch string. Delimiter sets are exact original
// literals: first "=\n", second "\n". No general caller-selected grammar.
bool nextTextToken29FFA0(bool first,std::optional<std::size_t>& returned)noexcept {
    returned.reset();sourcePC_=0x29FFB0u;
    if(!textTokenNext_)return true;
    const auto start=*textTokenNext_;textTokenCurrent_=start;
    const auto delimiter=[first](std::uint8_t ch)noexcept{return ch=='\n'||(first&&ch=='=');};
    std::size_t split=start;
    for(;;++split){
        sourcePC_=0x29FFCCu; //309B40 strpbrk; first unproved source read.
        if(split>=1024||!textFieldScratch_.known[split])return false;
        const auto ch=textFieldScratch_.value[split];
        if(!ch){textTokenNext_.reset();returned=start;return true;}
        if(delimiter(ch))break;
    }
    textTokenNext_=split+1;textFieldScratch_.put(split,0); //29FFE4..EC.
    sourcePC_=0x29FFF4u;
    if(*textTokenNext_>=1024||!textFieldScratch_.known[*textTokenNext_])return false;
    if(!textFieldScratch_.value[*textTokenNext_]){
        textTokenNext_.reset();returned=start;return true; //2A0004.
    }
    for(;;){
        sourcePC_=0x2A001Cu;
        if(*textTokenNext_>=1024||!textFieldScratch_.known[*textTokenNext_])return false;
        const auto ch=textFieldScratch_.value[*textTokenNext_];
        //Original delimiter loop compares its terminatingNUL too. After a
        //repeated delimiter it CAN skip a NUL and read retained/unknown tail.
        //Do not silently repair that path with host strtok semantics.
        if(ch!=0&&!delimiter(ch))break;
        ++*textTokenNext_;
    }
    sourcePC_=0x2A0078u;returned=start;return true;
}
bool textSectionForFieldCurrent(bool environment)const noexcept {
    return textConfigCurrent()&&textSelectionReturned_&&!textSelectBounded_&&sectionReturned_&&
        (environment?sectionAttempts_==3:(sectionAttempts_==1||sectionAttempts_==2))&&
        source_->child24==textChildTable_.get()&&source_->selected20==textChildCapacity_&&
        textPointersWritten_==textChildCapacity_&&textChildTable_&&
        std::memcmp(source_->name28.data(),environment?"Environment":"SceneInfo",environment?12u:10u)==0;
}
bool findTextField158240(bool environment,const char* requested,std::string_view& value,bool& found)noexcept {
    found=false;value={};sourcePC_=0x158648u;
    if(!textSectionForFieldCurrent(environment)||!requested)return false;
    //These are trusted exact caller literals, not arbitrary source-file bytes.
    const auto requestLength=std::strlen(requested);
    if(requestLength>=1024)return false;
    Text::Bytes query;
    for(std::size_t i=0;i<=requestLength;++i)query.put(i,static_cast<std::uint8_t>(requested[i]));
    //Fresh original158240 stack occurrence: do not carry knowledge between
    //different invocations. Within THIS row loop overwritten prefixes retain
    //known earlier-row tails, just like the original reused stack buffers.
    textFieldScratch_.known.fill(false);textFieldKey_.known.fill(false);
    const auto count=source_->selected20;
    for(std::uint32_t row=0;(count&0x80000000u)==0&&row<count;++row){
        sourcePC_=0x1586D0u;
        if(row>=textChildCapacity_||row>=textPointersWritten_||!textChildFilled_||
           !textChildFilled_[row]||!textChildData_||!textChildLengths_||!textChildOffsets_)return false;
        const auto length=textChildLengths_[row];
        if(length>=1024||textChildOffsets_[row]!=row*1024u)return false;
        const auto* data=static_cast<const std::uint8_t*>(textChildData_.get())+textChildOffsets_[row];
        //Only copied prefix+NUL is source-known; no raw tail is inspected.
        for(std::size_t i=0;i<length;++i)textFieldScratch_.put(i,data[i]);
        textFieldScratch_.put(length,0); //15871C original copy terminator.
        textTokenNext_=length?std::optional<std::size_t>(0):std::nullopt; //29FF50.
        std::optional<std::size_t> key,valueStart;
        if(!nextTextToken29FFA0(true,key))return false;
        sourcePC_=0x1587ACu;if(!key)return false; //Original strlen(NULL) boundary.
        std::size_t keyLength=0;
        if(!textKnownLength(textFieldScratch_,*key,keyLength,0x1587ACu))return false;
        for(std::size_t i=0;i<keyLength;++i)textFieldKey_.put(i,textFieldScratch_.value[*key+i]);
        textFieldKey_.put(keyLength,0); //1587E8.
        if(!nextTextToken29FFA0(false,valueStart))return false;
        if(!valueStart)continue; //158800: skip before comparing key.
        const auto compared=Text::compareKnown308F00(textFieldKey_,query);sourcePC_=compared.pc;
        if(compared.stop!=Text::Stop::complete)return false;
        if(compared.difference)continue;
        auto begin=*valueStart;sourcePC_=0x15881Cu;
        if(begin>=1024||!textFieldScratch_.known[begin])return false;
        if(textFieldScratch_.value[begin]=='"')++begin; //At most one leading quote.
        std::size_t size=0;
        if(!textKnownLength(textFieldScratch_,begin,size,0x158838u))return false;
        if(size&&textFieldScratch_.value[begin+size-1]=='"'){
            textFieldScratch_.put(begin+size-1,0);--size; //At most one trailing quote.
        }
        value=std::string_view(reinterpret_cast<const char*>(textFieldScratch_.value.data()+begin),size);
        found=true;fieldCopyPC_=0x1588B4u;sourcePC_=0x1588B4u;
        return true; //Actual destination copy remains caller-owned (a3=-1).
    }
    //Original text missing path writes empty string, unlike binary count>0
    //missing. Callers must perform that store for externally owned outputs.
    sourcePC_=0x1588E4u;return true;
}

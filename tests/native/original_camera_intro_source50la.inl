//Private SAME controller/native intro backing. This is explicit late source
//publication under native prewarming; never a second emitter, transport, cue
//array, Entity attachment or actual guest allocator/file-manager execution.
struct NativeIntro177750 {
    bool attempted=false,completed=false,rowsPublished=false,withdrawn=false;
    bool childrenAttempted=false,childrenComplete=false;
    unsigned initializedRows=0,mappedEvents=0;
    std::uint32_t pc=0x177750;
} nativeIntro177750_;
bool nativeIntroBackingCurrent()const noexcept {
    const auto* audio=actual_.taIntroAudio;
    return audio&&audio->taIntroNativeReserved_&&!audio->taIntroNativeWithdrawn_&&
        audio->taIntroExpectedController_==&coldController_.entity&&audio->taIntroBackingCurrent()&&
        actual_.taIntroAuthoring&&
        audio->coldGroups_[static_cast<std::size_t>(RaceAudioGroup50IC::Announcer120)].taIntroAuthoring==actual_.taIntroAuthoring&&
        nativeTaIssuance_&&
        actual_.taIntroAuthoring->providerIdentity()==nativeTaIssuance_->provider.get();
}
// Read-only check inside the existing audio friend; no emitter capability escapes.
bool nativeIntroFirstChildCurrent()const noexcept {
    return actual_.taIntroAudio&&
        actual_.taIntroAudio->taIntroEntity80()==coldController_.entity.firstChild18;
}
bool adoptNativeIntro177750(std::uint32_t& pc)noexcept {
    auto& n=nativeIntro177750_;auto& c=coldController_;pc=n.pc;
    if(!onOwnerThread()||n.attempted||!nativeRaOwnedPreparationAttempted_||
       !nativeAttached6C_.completed||nativeAttached6C_.pc!=0x175038||
       !nativeCamera74ConsumersCurrent()||!nativeIntroBackingCurrent()||
       c.script5C||c.children64[7]||taLineCount_)return false;
    n.attempted=true;
    //177778's fresh source80 is null. Adopt the actual native prewarmed
    //2921C0/events owner at1777B8, not a fabricated nonnull skip of its ctor.
    pc=n.pc=0x1777b8;c.children64[7]=actual_.taIntroAudio->taIntroEntity80();
    if(!c.children64[7])return false;
    //Immutable original source bytes replace1577D0's native file adapter,
    //not the original path-global36AFE0/file-manager lifetime. No fake handle.
    //Native event construction was done once;292300/2925A0 are not replayed.
    pc=n.pc=0x177894;
    if((c.halfA8Known&0x1f0u)!=0x1f0u)return false;
    const auto count=(c.halfA8Value>>4)&31u;
    if(!count||count>=16u||count>taLines_.size())return false;
    //Always-replace5C source store: same fixed backing is the explicit native
    //allocation policy, not an original allocator result or reused old list.
    pc=n.pc=0x17789c;c.script5C=taLines_.data();n.rowsPublished=true;taLineCount_=count;
    for(unsigned i=0;i<count;++i){
        pc=n.pc=0x1778c8;taLines_[i].eventKey=-1;
        pc=n.pc=0x1778d4;taLines_[i].span=0;++n.initializedRows;
    }
    pc=n.pc=0x177900;
    const auto& authoring=*actual_.taIntroAuthoring;
    if(authoring.trackId()!="RA"||!nativeTaIssuance_||
       authoring.providerIdentity()!=nativeTaIssuance_->provider.get())return false;
    const auto script=authoring.originalScriptBytes();
    //The exact authenticated98-byte RA member contains NumberOfLines=2.
    //Read that source scalar, not eventCount as a surrogate for the parser.
    constexpr char prefix[]="[General]\r\nNumberOfLines = ";
    constexpr std::size_t valueOffset=sizeof(prefix)-1;
    pc=n.pc=0x177930;
    if(script.size!=98u||!script.data||std::memcmp(script.data,prefix,valueOffset)||
       script.data[valueOffset]!='2'||script.data[valueOffset+1]!='\r'||
       script.data[valueOffset+2]!='\n')return false;
    const unsigned lines=script.data[valueOffset]-'0';
    if(lines!=authoring.eventCount())return false; //Backing corroboration only.
    for(unsigned i=1;i<=lines;++i){
        pc=n.pc=0x17797c;
        const auto* row=authoring.event(i);
        if(!row||row->eventId!=i)return false;
        //Exact hash-pinned decimal-prefix reads: RA1,2,3,4→1 and5,6,7→5.
        //Span is absent and source default0; do not fabricate bytes2..7.
        if(row->cameraNumber&&row->cameraNumber<=count){
            pc=n.pc=0x1779cc;taLines_[row->cameraNumber-1].eventKey=static_cast<std::int8_t>(i);
            pc=n.pc=0x1779e4;taLines_[row->cameraNumber-1].span=row->span;
        }
        ++n.mappedEvents;
    }
    pc=n.pc=0x177a1c; //Original temporary parser destructor; native immutable owner retained.
    //Same prewarmed real141960 attachment; no second child splice/count change.
    pc=n.pc=0x177a40;
    auto* e=actual_.taIntroAudio->taIntroEntity80();
    if(!e||c.children64[7]!=e||e!=c.entity.firstChild18||e->parent1C!=&c.entity||
       e->previous10||e->next14||e->firstChild18||!e->reference00)return false;
    if(!actual_.taIntroAudio->publishNativeTaIntro177750(c.entity))return false;
    n.completed=true;pc=n.pc=0x177a4c;return true;
}
bool nativeIntroSourceCurrent()const noexcept {
    if(!nativeIntro177750_.attempted)return !nativeRaOwnedPreparationAttempted_;
    const auto& n=nativeIntro177750_;
    return n.completed&&!n.withdrawn&&n.rowsPublished&&
        coldController_.script5C==taLines_.data()&&n.initializedRows==taLineCount_&&
        actual_.taIntroAudio&&actual_.taIntroAudio->taIntroReady()&&
        coldController_.children64[7]==actual_.taIntroAudio->taIntroEntity80();
}
bool withdrawNativeIntroSource()noexcept {
    auto& n=nativeIntro177750_;
    if(!n.attempted)return true;
    if(!onOwnerThread())return false;
    if(n.withdrawn)return !coldController_.script5C&&!coldController_.children64[7];
    if(n.rowsPublished&&coldController_.script5C!=taLines_.data())return false;
    if(coldController_.children64[7]&&(!actual_.taIntroAudio||
       coldController_.children64[7]!=actual_.taIntroAudio->taIntroEntity80()))return false;
    //Withdraw consumers before fallible event/transport/Entity destruction.
    n.withdrawn=true;coldController_.script5C=nullptr;coldController_.children64[7]=nullptr;
    if(actual_.taIntroAudio)actual_.taIntroAudio->taIntroNativeWithdrawn_=true;
    return true;
}
bool completeNativeControllerChildren177750(std::uint32_t& pc)noexcept {
    auto& n=nativeIntro177750_;pc=n.pc;
    if(!onOwnerThread()||n.childrenAttempted||!nativeAttached6C_.completed||
       !nativeRaOwnedPreparationAttempted_)return false;
    n.childrenAttempted=true;
    if(nativeAttached6C_.pc==0x175038){if(!adoptNativeIntro177750(pc))return false;}
    else if(nativeAttached6C_.pc!=0x175044)return false;
    pc=n.pc=0x175044;
    if(!constructNativeRaTv78()){
        if(actual_.taIntroAudio)actual_.taIntroAudio->taIntroNativeWithdrawn_=true;
        pc=n.pc=taFailurePC_;return false;
    }
    //78 is published175064 in the next7C allocation delay slot.
    pc=n.pc=0x175060;
    if(!constructNativeRaStart7C()){
        if(actual_.taIntroAudio)actual_.taIntroAudio->taIntroNativeWithdrawn_=true;
        pc=n.pc=taFailurePC_;return false;
    }
    n.childrenComplete=true;pc=n.pc=0x175080;return true;
}

//Compatibility for the NEVER-source-adopted standalone native route only.
//This retains its former native cue preparation, without replaying the real
//emitter/event construction or attachment now held in prewarmed custody.
bool publishNeverAdoptedNativeIntro()noexcept {
    if(nativeRaOwnedPreparationAttempted_||nativeIntro177750_.attempted)return false;
    auto* audio=actual_.taIntroAudio;
    if(!audio||!audio->taIntroNativeReserved_)return true; //Other courses unchanged.
    if(audio->taIntroNativePublished_)return !audio->taIntroNativeWithdrawn_;
    if(!onOwnerThread()||!nativeIntroBackingCurrent()||!taCuts_||
       coldController_.script5C||coldController_.children64[7])return false;
    taLineCount_=taCuts_->cutCount();
    if(!taLineCount_||taLineCount_>taLines_.size()||
       actual_.taIntroAuthoring->trackId()!=taCuts_->trackId())return false;
    for(std::size_t i=0;i<taLineCount_;++i)taLines_[i]={-1,0};
    for(unsigned i=1;i<=actual_.taIntroAuthoring->eventCount();++i){
        const auto* row=actual_.taIntroAuthoring->event(i);
        if(!row||row->eventId!=i)return false;
        if(row->cameraNumber&&row->cameraNumber<=taLineCount_)
            taLines_[row->cameraNumber-1]={static_cast<std::int8_t>(row->eventId),row->span};
    }
    coldController_.script5C=taLines_.data();
    coldController_.children64[7]=audio->taIntroEntity80();
    return audio->publishNativeTaIntro177750(coldController_.entity);
}

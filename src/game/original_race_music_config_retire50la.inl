// Included inside rr::game after the complete Impl. Only the genuine positive
//47-entry loop reaches this automatic Config destruction and local load return.
bool OriginalRaceMusicManager50LA::finishSourceMusicLoad2C0E08(
    const ::original_main_entry50KG::Application& app,OriginalRaceConstructor50LA& race,
    OriginalRaceMusicManagerDiagnostic50LA& d)noexcept {
    if(!impl_||!impl_->sourceOnly||!impl_->sourceChild)
        return Impl::stop(d,0x2c0e08,"Actual original Music child absent");
    auto& p=*impl_;auto& child=*p.sourceChild;auto& retired=child.configDestruction;
    const auto stop=[&](std::uint32_t pc,const char* why)noexcept {
        p.sourcePC=child.pc=pc;return Impl::stop(d,pc,why);};
    if(!p.componentCurrent()||race.music10C_.get()!=this||race.application_!=p.application||
       race.application_!=app.entities()||child.pc!=0x2c0e08||!child.metadataAttempted||
       child.count34!=47u||child.songOrdinal!=48u||child.lookupReturns!=47u||
       !child.config60||!child.configFile||child.configOpenReturn!=1u||
       child.sourceSongs58!=child.songEntries58.get()||!child.songs58Published||
       retired.entered||child.load2C0B00Return||child.initialize2C0A60Returned)
        return stop(0x2c0e08,"Original positive Music Config destruction occurrence unavailable");
    for(const auto& song:child.songLookups)
        if(!song.copied||!song.occurrence||!song.index||song.sourceReturn!=1u)
            return stop(0x2c0e08,"Original completed song lookup ownership unavailable");
    auto& config=*child.config60;auto& fd=child.configFileDiagnostic;
    retired.flag=-1;
    retired.savedResultS0=1u;retired.delayValueWritten=true; //2C0E0C BEFORE1573B0.
    retired.entered=true;child.pc=p.sourcePC=0x1573b0;
    if(config.file00!=-1){
        retired.flushEntered=true;std::int32_t returned=0;
        if(!child.configFile->flush12F290(&config,config.file00,returned,fd))
            return stop(fd.sourcePC,"Original Music Config destructor flush remains bounded");
        retired.flushReturn=returned; //Do not erase a returned flush if close later bounds.
        retired.closeEntered=true;
        if(!child.configFile->close131040(&config,config.file00,fd))
            return stop(fd.sourcePC,"Original Music Config destructor close remains bounded");
        retired.closeReturn=1;retired.closeReturned=true;config.file00=-1; //157404 ONLY after both returns.
    }
    //15740C->157CB0. The binary Config never owns a selected-text child.
    if(config.child24)return stop(0x157cc8,"Original selected-text child destruction is outside this cohort");
    config.selected20=0;config.child24=nullptr;retired.selectedClearReturned=true; //157CE4/E8.
    if(config.text1C)return stop(0x157420,"Original text allocation destruction is outside this binary cohort");
    //Null text1C leaves count14=48. Text caches were not populated by getters.
    if(config.cacheCount438)return stop(0x15743c,"Original populated text-cache destruction is not joined");
    config.cacheCount438=0; //157478.
    const auto release=[&](const void* pointer,std::unique_ptr<std::uint8_t[]>& backing,
            auto& receipt,std::uint32_t callPC)noexcept {
        receipt.identity=pointer;receipt.callPC=callPC;receipt.entered=true;
        if(!pointer||pointer!=backing.get())return stop(callPC,"Original free lacks its exact native allocation owner");
        if(!assets::OriginalConfigProcessGlobals50LA::reentrancyCurrentNative())
            return stop(0x305734,"Original Music free process reentrancy custody unavailable");
        //Matched native new[]/delete[] adaptation. Guest heap bins/locks and
        //allocator addresses are not executed or inferred from native storage.
        backing.reset();receipt.returned=true;return true;
    };
    if(config.names4CC){
        if(!release(config.names4CC,child.names4CC,retired.names,0x157488))return false;
        config.names4CC=nullptr; //157490, after free returned.
    }
    if(config.index4BC){
        if(!release(config.index4BC,child.index4BC,retired.index,0x1574a0))return false;
        config.index4BC=nullptr; //1574A8. indexBytes4C0 remains384.
    }
    if(config.block4C4){
        if(!release(config.block4C4,child.block4C4,retired.block,0x1574b8))return false;
        child.blockKnown.reset(); //Native knownness lifetime only.
        //No source null store or capacity reset: keep the historical dead
        //block4C4 identity, never dereference it after this lifetime ends.
    }
    //Signed low16(-1)<=0 skips object deletion;1573B0 returns this same Config.
    retired.returnedThis=&config;retired.returned=true;retired.automaticLifetimeEnded=true;
    //Actual loose131040 already retired device/member and ended their borrows.
    //Release native root custody only; do not retire historical slots again.
    if(!child.configFile->closeNative(fd))return stop(0,"Native Music terminal file-custody release remains bounded");
    retired.nativeFileRetired=true;child.configFile.reset();
    child.load2C0B00Return=*retired.savedResultS0; //2C0E10 copies the earlier delay-slot1.
    child.pc=p.sourcePC=0x2c0e2c;d={child.pc,nullptr};return true;
    //Local2C0B00 return only. Its concrete caller still owns2C0AA4/74 and
    //must stop before any unresolved manager attachment or1A3C00 return.
}

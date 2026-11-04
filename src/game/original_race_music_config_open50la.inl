// Included inside rr::game after Impl. Only the retained same-App stream's
// first2C0B98 call can enter. No native Config parser or caller-supplied handle.
bool OriginalRaceMusicManager50LA::openSourceMusicConfig2C0B98(
    const ::original_main_entry50KG::Application& app,OriginalRaceConstructor50LA& race,
    OriginalRaceMusicManagerDiagnostic50LA& d)noexcept {
    if(!impl_||!impl_->sourceOnly||!impl_->sourceChild)
        return Impl::stop(d,0x2c0b98,"Actual original music child absent");
    auto& p=*impl_;auto& child=*p.sourceChild;
    const auto stop=[&](std::uint32_t at,const char* why)noexcept {
        p.sourcePC=child.pc=at;return Impl::stop(d,at,why);};
    if(!p.componentCurrent()||race.music10C_.get()!=this||race.application_!=p.application||
       race.application_!=app.entities()||child.pc!=0x2c0b98||!child.counted||
       !child.constructorReturned||!child.published164||!child.loadEntered||!child.config60||
       child.configOpenAttempted||child.configFile||child.filename750Known!=18u||
       std::memcmp(child.filename750.data(),"audio\\racemus.jub",18u)!=0)
        return stop(0x2c0b98,"Original first music Config occurrence unavailable");
    // The private Application::populateConsumedRaceResources1AA5A0 issuer
    // enrolled this very domain/context from its canonical owners. Do not call
    // Application::standardMaterialDomain(): that native getter can PREPARE a
    // missing domain and would invent work before the source131230 call.
    if(!race.resourcesPrepared_||!race.resourceDomain_||!race.resourceDomain_->prepared()||
       !race.resourceContext_||!race.resourceContext_->live()||
       race.resourceContext_->materialApplication()!=app.materialApplication||
       !race.resourceDomain_->resolver3E1A70())
        return stop(0x1578d0,"Same-Application enrolled FileManager custody unavailable");
    auto& config=*child.config60;
    // Fresh157320 makes all four cleanup branches1577E8..15785C skip. This
    // intentionally does not implement reuse or the later .juk fallback open.
    if(config.text1C||config.file00!=-1||config.index4BC||config.block4C4||
       config.names4CC||config.count14||config.opened0C||config.binary08||config.mode10)
        return stop(0x1577e8,"Automatic Config is not its reached fresh constructor");
    child.configOpenAttempted=true;
    try {child.configFile.reset(new assets::OriginalMusicSourceFile50LA(&app,this));}
    catch(...){return stop(0x1578d0,"Native file owner backing unavailable; no source open return");}
    auto& fd=child.configFileDiagnostic;
    std::int64_t returned=-1; // This local is NOT published unless the call returned.
    if(!child.configFile->begin131230(race.resourceDomain_->resolver3E1A70(),&config,
            app.audioRoot,std::string_view(child.filename750.data(),child.filename750Known-1u),returned,fd))
        return stop(fd.sourcePC,"Original music FileManager open remains bounded; no Config result");
    config.file00=returned; //1578D8, including a genuine131230 signed-1 result.
    const auto failedOpenReturn=[&]()noexcept {
        child.configOpenReturn=0; //157ACC epilogue then2C0BA0 fallthrough.
        return stop(0x2c0bac,"Original Config returned0;2C0E40 conversion is not implemented");};
    if(config.file00==-1)return failedOpenReturn();
    if(!child.configFile->current(&config,config.file00))
        return stop(0x1578f8,"Returned original handle occurrence no longer current");
    config.field42C=0;config.opened0C=1;config.mode10=0; //1578F8/FC/157900.
    const auto read=[&](std::uint32_t call,void* out,std::size_t bytes,std::size_t& known)noexcept {
        if(child.configReadCount==child.configReads.size())
            return stop(call,"Native read receipt capacity exceeded; no source read result");
        auto& receipt=child.configReads[child.configReadCount++];
        receipt.callPC=call;receipt.requested=bytes;
        receipt.returned=child.configFile->read12FB30(&config,config.file00,out,bytes,receipt.copied,fd);
        known=receipt.copied; // Partial bytes remain known even on a bounded native stop.
        if(!receipt.returned)return stop(fd.sourcePC,"Original music Config read remains bounded");
        return true;
    };
    const auto word=[](const std::array<std::uint8_t,4>& b)noexcept {
        return std::uint32_t(b[0])|(std::uint32_t(b[1])<<8u)|
            (std::uint32_t(b[2])<<16u)|(std::uint32_t(b[3])<<24u);};
    if(!read(0x157914,child.magic48.data(),7,child.magicKnown))return false;
    child.magic48[6]=0;child.magicTerminatorWritten=true; //15792C delay store.
    if(child.magicKnown<6u)return stop(0x157928,"Original strcmp would consume unknown magic bytes");
    if(std::memcmp(child.magic48.data(),"BINARY",6)!=0){
        config.binary08=0; //157A9C, before unimplemented seek/text parser.
        return stop(0x157ab0,"Original nonbinary Config seek/parser branch is not implemented");
    }
    config.binary08=1; //15793C, before reading uninitialized stack version54.
    if(!read(0x157950,child.version54.data(),4,child.versionKnown))return false;
    if(child.versionKnown!=4u)return stop(0x157958,"Original version load would consume unknown bytes");
    const auto closeFailed=[&](std::uint32_t call)noexcept {
        if(!child.configFile->close131040(&config,config.file00,fd))
            return stop(fd.sourcePC?fd.sourcePC:call,"Original Config failure close remains bounded");
        config.file00=-1; //15797C or1579C0; opened/binary fields stay reached.
        return failedOpenReturn();
    };
    if(word(child.version54)!=1u)return closeFailed(0x157970);
    if(!read(0x157998,child.sections58.data(),4,child.sectionsKnown))return false;
    if(child.sectionsKnown!=4u)return stop(0x1579a0,"Original section count load would consume unknown bytes");
    const auto sections=word(child.sections58);
    if(!sections)return closeFailed(0x1579b4);
    config.count14=sections; //1579CC. Authenticated corpus has48, not a substitute read.
    if(sections!=child.nameKnown.size())
        return stop(0x1579dc,"Source section count exceeds this authenticated Config cohort");
    // Typed host allocation adapts original305700 backing. Host failure is a
    // bounded allocation, NOT invented guest NULL and its source close/return0.
    try {child.names4CC.reset(new std::uint8_t[std::size_t(sections)*1024u]);}
    catch(...){return stop(0x1579dc,"Native name storage unavailable; original malloc return unknown");}
    config.names4CC=child.names4CC.get(); //1579E4; no initialization of name tails.
    for(;child.nameOrdinal<sections;++child.nameOrdinal){
        child.length5C.fill(0);child.lengthZeroed=true; //157A18, unlike version/count.
        std::size_t supplied=0;
        if(!read(0x157a30,child.length5C.data(),4,supplied))return false;
        const auto length=word(child.length5C); // Unwritten suffix was explicitly zeroed.
        if(length>1024u)return stop(0x157a4c,"Original name length exceeds safe native slot extent");
        if(!read(0x157a4c,child.names4CC.get()+std::size_t(child.nameOrdinal)*1024u,
                length,child.nameKnown[child.nameOrdinal]))return false;
        //157A58 and157A60 advance ordinal and1024-byte destination even on a
        //returned short read. No invented terminator or initialized tail.
    }
    config.indexBytes4C0=sections*8u; //157A68.
    try {child.index4BC.reset(new std::uint8_t[config.indexBytes4C0]);}
    catch(...){return stop(0x157a6c,"Native index storage unavailable; original malloc return unknown");}
    config.index4BC=child.index4BC.get(); //157A74. No guest NULL-result claim.
    if(!read(0x157a8c,child.index4BC.get(),config.indexBytes4C0,child.indexKnown))return false;
    // Original157A94/98 returns1 regardless of the returned read byte count.
    // Keep that source result separate from which index bytes are actually known.
    child.configOpenReturn=1;
    return stop(0x2c0be4,"Original binary Config open returned1; Songs selection is not implemented");
}

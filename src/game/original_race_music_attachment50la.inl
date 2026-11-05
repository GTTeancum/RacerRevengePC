// Included inside rr::game after complete Impl. This is the real same-manager
//post-child continuation, never an entry point from native audio readiness.
bool OriginalRaceMusicManager50LA::continueSourceMusicManager1A3C74(
    const ::original_main_entry50KG::Application& app,OriginalRaceConstructor50LA& race,
    OriginalRaceMusicManagerDiagnostic50LA& d)noexcept {
    if(!impl_||!impl_->sourceOnly||!impl_->sourceChild)
        return Impl::stop(d,0x1a3c74,"Actual source Music child is absent");
    auto& p=*impl_;auto& child=*p.sourceChild;auto& tail=p.sourceTail;
    const auto stop=[&](std::uint32_t pc,const char* why)noexcept {
        p.sourcePC=pc;return Impl::stop(d,pc,why);};
    if(!p.componentCurrent()||!p.constructed||!p.sourceLoadAttempted||
       race.music10C_.get()!=this||race.application_!=p.application||race.application_!=app.entities()||
       p.sourcePC!=0x1a3c74||child.pc!=0x2c0af4||child.load2C0B00Return!=1u||
       !child.initialize2C0A60Returned||child.initialize2C0A60Return!=&child.entity||
       !child.configDestruction.returned||!child.configDestruction.automaticLifetimeEnded||
       !child.configDestruction.nativeFileRetired||child.configFile||
       !child.published164||!p.fields.child164||*p.fields.child164!=&child.entity||
       tail.argumentsPrepared||tail.attachmentEntered||tail.attachmentReturned||
       tail.allocationEntered||tail.events158||tail.nativeChildDetached||tail.nativeEventsRetired||
       !p.fields.events158||*p.fields.events158||p.fields.word154||p.fields.wordA8||p.fields.wordAC||
       p.fields.word150||p.nativeEvents||!p.registration.expired()||p.music||p.announcer||
       p.live128_12C_130!=&p.source128_12C_130||!p.sourceGroups||p.sourceGroups!=race.raceAudio_||
       p.sourceGroups->sourceMusicBorrower_!=this||!p.aural||!p.aural->current()||
       p.aural->issuer_!=&app||p.aural->entities_!=p.application||
       !p.aural->interfaceA0_||!*p.aural->interfaceA0_||
       p.nativeInterface44!=&**p.aural->interfaceA0_||child.backend3C!=p.nativeInterface44)
        return stop(0x1a3c74,"Original post-child manager occurrence unavailable");
    auto& e=child.entity;
    if(!child.counted||e.reference00!=1u||e.kind!=OriginalWreckComponentKind50LA::music2C0980||
       e.sessionGeneration!=p.component.sessionGeneration||
       e.objectKey!=std::uint64_t(reinterpret_cast<std::uintptr_t>(&e))||
       e.previous10||e.next14||e.firstChild18||e.parent1C||e.mask20||e.descendants24||
       e.nativeFlags29||e.category28!=0xffu||(e.flags29&15u)!=3u||
       !p.application->entityAuthority_.canRetire(e))
        return stop(0x1a3c74,"Actual fresh Music child topology is unavailable");
    //1A3C74 reloads published164, not the ignored2C0A60 return.1A3C78
    //sets a2=2;1A3C80's delay slot supplies the same manager as a0.
    tail.childArgument=*p.fields.child164;tail.maskArgument=2u;
    tail.parentArgument=&p.component;tail.argumentsPrepared=true;
    tail.attachmentEntered=true;p.sourcePC=0x141960;
    const auto attached=p.application->entityAuthority_.attachChildren(
        *tail.parentArgument,*tail.childArgument,*tail.maskArgument,p.application->attachmentLock423F58());
    if(attached.status!=OriginalWreckComponentStatus50LA::complete)
        return stop(attached.sourcePC,"Actual source Music attachment remains bounded");
    //141960 returns the actual child without AddRef, callbacks or activity writes.
    tail.returnedChild=tail.childArgument;tail.attachmentReturned=true;
    p.sourcePC=0x1a3c84;
    if(!p.componentCurrent())return stop(0x1a3c84,"Source Music attachment identity changed");
    //1A3C84 ->305700, call delay argument0x280. Same persistent38544C
    //read at30570C; native new[] adapts allocation, not guest heap machinery.
    tail.allocationBytes=0x280u;tail.allocationEntered=true;p.sourcePC=0x305700;
    using G=assets::OriginalConfigProcessGlobals50LA;
    if(!G::reentrancyCurrentNative()||G::state().reent38544C!=G::StdioIdentity::reent385160)
        return stop(0x30570c,"Original Music event allocation process custody unavailable");
    tail.allocationReentRead=true; //Same process's actual symbolic reent385160 argument.
    try {tail.events158.reset(new std::uint8_t[*tail.allocationBytes]);}
    catch(...){return stop(0x305990,"Native event backing unavailable; guest allocation result remains unknown");}
    tail.allocationReturn=tail.events158.get();tail.allocationReturned=true;
    p.fields.events158=tail.allocationReturn;tail.eventsPublished=true; //1A3C8C.
    p.fields.word154=32u; //1A3C94. No write to the32 unknown20-byte entries.
    p.fields.wordA8=0u; //1A3C98.
    p.fields.wordAC=0x43e10000u; //1A3CA0, original float bits450.0.
    tail.soundOrdinal=0u;tail.soundLoopOwner=&p.component; //1A3CA4/A8.
    p.sourcePC=0x1a3cac;
    return constructSourceSound1A3CAC(app,race,d);
    //Sound setup, listener1A4620, manager1A3C00 return and playback remain
    //unjoined. The raw source event backing is never the native Session FIFO.
}

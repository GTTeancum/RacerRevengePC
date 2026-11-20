// Included inside rr::game after the complete manager Impl. No independent
// entry point, native Scope adoption, music playback or invented open result.
OriginalRaceMusicManager50LA::OriginalRaceMusicManager50LA(
    std::shared_ptr<OriginalApplicationEntities50LA> app,std::uint64_t generation)
    :impl_(new Impl(std::move(app),generation,{},true)){}

bool OriginalRaceMusicManager50LA::beginSourceAfterLoadingCamera1A6C90(
    const ::original_main_entry50KG::Application& app,OriginalRaceConstructor50LA& race,
    const std::shared_ptr<platform::OriginalApplicationAural50KG>& aural,std::uint32_t& pc)noexcept {
    pc=0x1a9598;
    // Only Application's genuine1A9750 lexical return calls this private leaf.
    // Its own prior same-App28C and display/array checks are not replaced here.
    const auto& camera=race.loadingCamera34_;
    if(race.resultsFeedbackThread_!=std::this_thread::get_id()||race.application_!=app.entities()||
       race.profile_!=app.riderProfile||!race.counted_||race.rolledBack_||!race.nativeIntroRaceEntity()||
       // 1A39F0 runs before the 1A9EE0 track load (PLAN 2.1 step 3 before 5).
       !race.world74_||race.world74_->trackData()||
       race.cameraNativeAdapter34_||race.cameraInitStep_!=OriginalRaceConstructor50LA::CameraInitStep::nativePending||
       !camera.suffixReturned||camera.terminal||!camera.arrayPublished||camera.arrayWithdrawn||
       camera.stopPC!=0x1a9598||!race.loadingCameraSourceNullSuffixCurrent()||
       (race.flags1B0KnownMask_&3u)!=3u||(race.flags1B0KnownValue_&3u)!=1u||
       !race.endHud_.expired()||race.music10C_)return false;
    //1A9598 literal V0=1, restored ra/s0 and1A95A8 stack release return to
    //1A6C90. That caller ignores V0. Native allocation failure is a stop,
    //not an invented guest null result that skips1A3C00.
    pc=0x1a6c90;
    try {
        std::unique_ptr<OriginalRaceMusicManager50LA> next(
            new OriginalRaceMusicManager50LA(race.application_,race.entity_.sessionGeneration));
        if(!next->constructed()){pc=0x1a39f0;return false;}
        race.music10C_=std::move(next); //1A6CAC, before reading flags for load.
    }catch(...){return false;}
    OriginalRaceMusicManagerDiagnostic50LA d;
    const bool done=race.music10C_->loadSourceMusicChild1A3C00(app,race,aural,d);
    pc=d.sourcePC;return done; //Local child load may return; manager1A3C00 remains bounded.
}

bool OriginalRaceMusicManager50LA::loadSourceMusicChild1A3C00(
    const ::original_main_entry50KG::Application& app,OriginalRaceConstructor50LA& race,
    const std::shared_ptr<platform::OriginalApplicationAural50KG>& aural,
    OriginalRaceMusicManagerDiagnostic50LA& d)noexcept {
    d={};
    if(!impl_||!impl_->sourceOnly)return Impl::stop(d,0x1a3c00,"Not an original music occurrence");
    auto& p=*impl_;
    const auto stop=[&](std::uint32_t at,const char* why)noexcept {
        p.sourcePC=at;if(p.sourceChild)p.sourceChild->pc=at;return Impl::stop(d,at,why);};
    if(!p.componentCurrent()||!p.constructed||p.sourceLoadAttempted||
       race.music10C_.get()!=this||race.application_!=p.application||race.application_!=app.entities()||
       p.live128_12C_130!=&p.source128_12C_130||!p.registration.expired()||p.music||p.announcer||
       (race.flags1B0KnownMask_&3u)!=3u)return stop(0x1a3c00,"Actual original music owner unavailable");
    p.sourceLoadAttempted=true;
    p.fields.count38=race.flags1B0KnownValue_&3u; //1A3C14.
    if(!aural||!aural->current())return stop(0x1a3c24,"Actual Application Aural absent;38 retained");
    const auto view=aural->inspect();
    if(!view.constructed||!view.interfaceKnown||!view.interfaceA0||
       p.application->auralNativeOwner_!=aural.get()||!p.application->aural00Live_||
       p.application->aural00_!=view.entity)
        return stop(0x1a3c30,"Actual Aural backend identity unavailable;38 retained");
    p.aural=aural;p.nativeInterface44=view.interfaceA0; // Typed owner of AuralA0+40, not guest arithmetic.
    p.sourcePC=0x1a3c34;
    try {p.sourceChild=std::make_unique<original_race_music_source50LA::Child>();}
    catch(...){return stop(0x1a3c34,"Native backing unavailable; original allocation result unknown");}
    auto& child=*p.sourceChild;
    //1A3C48/50/54 must read the SAME Application28C Race118. Caller is the
    //private same-App suffix; canonical six-group identity is checked here.
    auto* groups=race.raceAudio_;
    if(!groups||groups->sourceMusicBorrower_||!groups->coldStarted_||groups->coldRetiring_||
       groups->coldThread_!=p.thread||groups->coldApplication_!=p.application||
       groups->coldSession_!=p.component.sessionGeneration||race.raceGroupCount_!=6u||
       race.raceGroupsAttached_!=6u)
        return stop(0x1a3c54,"Actual same-Race music group unavailable");
    auto& group=groups->coldGroups_[static_cast<std::size_t>(RaceAudioGroup50IC::Music118)];
    if(race.raceGroups_[2]!=&group.entity||!group.constructed||!group.loaded||
       group.entity.kind!=OriginalWreckComponentKind50LA::entity141300||
       !group.enabled30||!group.allocated58||!group.logicalMembers4C||
       group.capacity54!=8u||group.growth50!=16u||group.count48||
       group.entity.reference00!=1u||group.entity.sessionGeneration!=p.component.sessionGeneration||
       group.entity.parent1C!=&race.entity_||!p.application->entityAuthority_.canRetire(group.entity)||
       group.slot40<0||std::size_t(group.slot40)>=p.application->soundGroups434A10_.size()||
       p.application->soundGroups434A10_[group.slot40]!=&group.entity)
        return stop(0x1a3c54,"Canonical Race118/global group40 identity unavailable");
    groups->sourceMusicBorrower_=this;p.sourceGroups=groups;
    auto& assertions=original_race_music_source50LA::vectorAssertions();
    if(assertions.thread!=p.thread)return stop(0x233f20,"Shared source vector owner thread mismatch");
    const auto key=std::uint64_t(reinterpret_cast<std::uintptr_t>(&child.entity));
    if(p.application->objectCount423F60()==std::numeric_limits<std::uint32_t>::max()||
       !p.application->entityAuthority_.reserveAdditional(1)||
       !p.application->entityAuthority_.canEnroll(child.entity,key))
        return stop(0x10285c,"Source child count/enrollment unavailable; manager retained");
    const auto made=initializeOriginalWreckComponent50LA(child.entity,
        OriginalWreckComponentKind50LA::music2C0980,p.component.sessionGeneration,key,true,
        p.application->objectCount423F60()); //2C09AC→233F40→10285C, a1=1.
    if(made.status!=OriginalWreckComponentStatus50LA::complete||
       !p.application->entityAuthority_.enroll(child.entity))std::terminate();
    child.counted=true;child.dispatch08=0x3cc790u; //10286C.
    child.word30=1;child.count34=0;child.dispatch08=0x3d1b30u; //102874/78,233F54.
    child.storage48.reset();child.vectorCount44=0;child.growth4C=0;child.capacity50=0;
    child.flags54KnownMask=1;child.flags54KnownValue=0; //235C80..AC only bit0.
    child.group38=1;child.backend3C=view.interfaceA0;child.mode40=5; //233F74/7C/84.
    //235CB0: two process lazy assertion owners. Actual positive initial1 and
    //constructor capacity0 make133660 return0 on either global-enabled arm.
    if(!assertions.guard8834){assertions.assertion434AB8=1;assertions.guard8834=true;}
    if(!assertions.guard8838){assertions.assertion434AC0=1;assertions.guard8838=true;}
    try {child.storage48.reset(new std::optional<std::uint32_t>[1]);}
    catch(...){return stop(0x235da8,"Native vector allocation unavailable; counted constructor prefix retained");}
    child.growth4C=16;child.capacity50=1;child.flags54KnownValue=1; //235DB0..DD0.
    //235CB0 returns1 from actual retained backing; no invented data entries.
    child.word58=0;child.word5C=0xffffffffu;child.word60=0xffffffffu;
    child.word64=0;child.word68=0;child.word30=0;child.word6C=0;child.word70=0;
    child.flags74KnownMask=0x7fu;child.flags74KnownValue=0; // Upper bit remains UNKNOWN.
    child.dispatch08=0x3d4d10u;child.word78=0;
    child.group38=static_cast<std::uint32_t>(group.slot40); //2C09C8/D0 nonnull branch.
    child.constructorReturned=true;
    p.fields.child164=&child.entity;child.published164=true; //1A3C60 BEFORE load.
    child.loadEntered=true;child.backend3C=view.interfaceA0; //2C0A8C.
    //2360F0(child44,5): preserve the old capacity1 unknown cell, publish new
    //backing before releasing old, then capacity5. No count change or zeroing.
    std::unique_ptr<std::optional<std::uint32_t>[]> expanded;
    try {expanded.reset(new std::optional<std::uint32_t>[5]);}
    catch(...){return stop(0x236120,"Native reserve unavailable; original reserve return unresolved");}
    expanded[0]=child.storage48[0];
    auto old=std::move(child.storage48);child.storage48=std::move(expanded);old.reset();
    child.capacity50=5;
    // Genuine2C0B00 automatic Config and exact original literal input.
    child.config60.emplace(); //2C0B24→157320; all unknown name tail remains unknown.
    std::string_view directory;
    if(!app.nativeAudioDirectory36AFE0().read36AFE0(app,directory))
        return stop(0x2c0b40,"Actual writable audio directory unavailable; Config retained");
    //Existing2C0B40 fixed%s%s filename adapter remains explicitly native.
    //It cannot establish source stdio initialization before the later exact
    //Song%d call. Preserve only genuine earlier initialization, or stop later.
    using G=assets::OriginalConfigProcessGlobals50LA;
    G::nativeMusicStdioAdapterBoundary(G::MusicSprintfAccess{});
    constexpr std::string_view name="racemus.juk";
    const auto length=directory.size()+name.size();
    if(length>=child.filename640.size())return stop(0x2c0b40,"Native pathname extent unavailable");
    std::size_t at=0;for(const char c:directory)child.filename640[at++]=c;
    for(const char c:name)child.filename640[at++]=c;
    child.filename640[at++]=0;child.filename640Known=at; //308C90 %s%s.
    std::memcpy(child.filename750.data(),child.filename640.data(),at);child.filename750Known=at; //309230.
    // Authenticated filename has exactly one dot.1169D0 returns that suffix;
    //309230 overwrites it with literal .jub, including its NUL.
    const auto dot=directory.size()+name.find('.');
    constexpr char extension[]={'.','j','u','b','\0'};
    std::memcpy(child.filename750.data()+dot,extension,sizeof(extension));
    child.filename750Known=dot+sizeof(extension);
    child.pc=p.sourcePC=0x2c0b98;
    if(!openSourceMusicConfig2C0B98(app,race,d))return false;
    //Only the genuine positive2C0B00 epilogue can reach this2C0AA4 branch.
    //The manager's caller must NOT see booltrue until1A3C00 itself returns.
    if(child.load2C0B00Return!=1u||!child.configDestruction.returned||
       !child.configDestruction.automaticLifetimeEnded||!child.configDestruction.nativeFileRetired||
       child.configFile||child.initialize2C0A60Returned)
        return stop(0x2c0aa4,"Actual original Music child load return unavailable");
    child.pc=p.sourcePC=0x2c0aac;
    //Read/modify/write preserves every other bit, including UNKNOWN bit7.
    child.flags74KnownValue=std::uint8_t((child.flags74KnownValue&~1u)|1u);
    child.flags74KnownMask|=1u; //2C0AC8 branch delay store.
    child.initialize2C0A60Return=&child.entity;child.initialize2C0A60Returned=true;
    child.pc=p.sourcePC=0x2c0af4; //Actual child-pointer return; no playback action.
    //The same concrete manager continues only after that genuine child return.
    //Keep the child's last PC at its own return; the parent now owns1A3C74.
    p.sourcePC=0x1a3c74;
    return continueSourceMusicManager1A3C74(app,race,d);
    //Failed first open still stops before unimplemented2C0E40 conversion.
}

bool OriginalRaceMusicManager50LA::retireSourceMusicChildNative(
    OriginalRaceMusicManagerDiagnostic50LA& d)noexcept {
    if(!impl_||!impl_->sourceOnly)return false;auto& p=*impl_;
    if(!p.counted&&!p.sourceChild&&!p.sourceGroups&&!p.sourceTail.events158&&!p.sourceTail.firstSound)return true;
    if(p.thread!=std::this_thread::get_id()||!p.componentCurrent())
        return Impl::stop(d,p.sourcePC,"Source music native retirement custody unavailable");
    if(p.sourceGroups&&p.sourceGroups->sourceMusicBorrower_!=this)
        return Impl::stop(d,p.sourcePC,"Source music group lifetime pin changed");
    if(p.sourceTail.events158&&(!p.sourceTail.allocationReturned||!p.sourceTail.eventsPublished||
       !p.fields.events158||*p.fields.events158!=p.sourceTail.events158.get()||
       p.sourceTail.allocationReturn!=p.sourceTail.events158.get()))
        return Impl::stop(d,p.sourcePC,"Source event allocation native custody changed");
    if(p.sourceTail.firstSound){auto& sound=*p.sourceTail.firstSound;auto& e=sound.entity;
        //Native cancellation of an UNPUBLISHED partial2C20F0 object. No source
        //slot10/2C2160/292940/291180 call, audio stop or serial rollback.
        if(sound.nativeIssuer!=this||!p.sourceTail.soundAllocationReturned||
           p.sourceTail.soundAllocationReturn!=&e||sound.constructorReturn||
           !p.fields.childrenC0[0]||*p.fields.childrenC0[0]||
           e.previous10||e.next14||e.firstChild18||e.parent1C||e.mask20||e.descendants24)
            return Impl::stop(d,p.sourcePC,"Unpublished source sound native custody changed");
        if(sound.counted){
            if(e.kind!=OriginalWreckComponentKind50LA::soundEmitter2C20F0||e.reference00!=1u||
               e.sessionGeneration!=p.component.sessionGeneration||
               e.objectKey!=std::uint64_t(reinterpret_cast<std::uintptr_t>(&e))||
               !p.application->objectCount423F60()||!p.application->entityAuthority_.canRetire(e))
                return Impl::stop(d,p.sourcePC,"Counted source sound native retirement unavailable");
            if(!p.application->entityAuthority_.retire(e))return false;
            --e.reference00;--p.application->objectCount423F60();sound.counted=false;
        }else if(e.reference00||e.objectKey||e.sessionGeneration||e.kind!=OriginalWreckComponentKind50LA::unbound)
            return Impl::stop(d,p.sourcePC,"Uncounted source sound identity changed");
        p.sourceTail.firstSound.reset();p.sourceTail.nativeSoundRetired=true;
    }
    if(p.sourceChild){auto& child=*p.sourceChild;auto& e=child.entity;
        // Release the FileManager borrower before Config/array/child storage.
        // This native cancellation is not1573B0 and cannot restore mode170.
        if(child.configFile){
            assets::OriginalStandardTextureDiagnostic50LA fd;
            if(!child.configFile->closeNative(fd))
                return Impl::stop(d,fd.sourcePC,"Source music file native cancellation unavailable");
            child.configFile.reset();
        }
        if(p.sourceTail.attachmentReturned&&!p.sourceTail.nativeChildDetached){
            //Native cancellation of this exact private topology, NOT a source
            //1A3AF0/2C0A00 occurrence, playback stop or reference release.
            const auto detached=p.application->entityAuthority_.detach(e,p.application->attachmentLock423F58());
            if(detached.status!=OriginalWreckComponentStatus50LA::complete)
                return Impl::stop(d,detached.sourcePC,"Source Music child native detachment unavailable");
            p.sourceTail.nativeChildDetached=true; //detach preserves child mask20=2.
        }
        if(child.counted){
            if(e.reference00!=1u||e.kind!=OriginalWreckComponentKind50LA::music2C0980||
               e.sessionGeneration!=p.component.sessionGeneration||
               e.objectKey!=std::uint64_t(reinterpret_cast<std::uintptr_t>(&e))||
               e.previous10||e.next14||e.firstChild18||e.parent1C||
               e.mask20!=(p.sourceTail.nativeChildDetached?2u:0u)||e.descendants24||
               !p.application->objectCount423F60()||!p.application->entityAuthority_.canRetire(e)||
               (child.published164&&(!p.fields.child164||*p.fields.child164!=&e)))
                return Impl::stop(d,p.sourcePC,"Detached source stream cannot retire");
            // Native cancellation, not2C0A00/2340C0 or a Config destructor.
            //This does not replay a returned child load or execute a source stream stop.
            if(!p.application->entityAuthority_.retire(e))return false;
            --e.reference00;--p.application->objectCount423F60();child.counted=false;
        }
        p.fields.child164.reset();p.sourceChild.reset();
    }
    if(p.sourceTail.events158){
        //Native disposal of this occurrence's raw allocation only. Preserve
        //154/A8/AC as history; withdraw158 knownness, not a source NULL store.
        p.sourceTail.events158.reset();p.sourceTail.nativeEventsRetired=true;
        p.fields.events158.reset();
    }
    if(p.sourceGroups){p.sourceGroups->sourceMusicBorrower_=nullptr;p.sourceGroups=nullptr;}
    return true;
}

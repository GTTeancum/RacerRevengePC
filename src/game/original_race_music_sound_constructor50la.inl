//Included inside rr::game after complete Impl. Only the genuine same-manager
//post-attachment call site reaches this first Effects110 emitter constructor.
bool OriginalRaceMusicManager50LA::constructSourceSound1A3CAC(
    const ::original_main_entry50KG::Application& app,OriginalRaceConstructor50LA& race,
    OriginalRaceMusicManagerDiagnostic50LA& d)noexcept {
    if(!impl_||!impl_->sourceOnly)return Impl::stop(d,0x1a3cac,"Original Music manager absent");
    auto& p=*impl_;auto& tail=p.sourceTail;
    const auto stop=[&](std::uint32_t pc,const char* why)noexcept {
        p.sourcePC=pc;if(tail.firstSound)tail.firstSound->pc=pc;return Impl::stop(d,pc,why);};
    if(!p.componentCurrent()||!p.constructed||race.music10C_.get()!=this||
       race.application_!=p.application||race.application_!=app.entities()||p.sourcePC!=0x1a3cac||
       !tail.attachmentReturned||tail.nativeChildDetached||tail.soundOrdinal!=0u||
       tail.soundLoopOwner!=&p.component||tail.soundAllocationEntered||tail.firstSound||tail.nativeSoundRetired||
       !tail.allocationReturned||!tail.allocationReentRead||!tail.eventsPublished||!tail.events158||
       !p.fields.events158||*p.fields.events158!=tail.events158.get()||
       p.fields.word154!=32u||p.fields.wordA8!=0u||p.fields.wordAC!=0x43e10000u||p.fields.word150||
       !p.fields.childrenC0[0]||*p.fields.childrenC0[0]||p.nativeEvents||!p.registration.expired()||
       p.music||p.announcer||p.live128_12C_130!=&p.source128_12C_130||
       !p.sourceGroups||p.sourceGroups!=race.raceAudio_||p.sourceGroups->sourceMusicBorrower_!=this)
        return stop(0x1a3cac,"Actual first source sound occurrence unavailable");
    //Actual1A3CB0 request0x250 ->2E8D90 ->305700. Positive native allocation
    //adapts guest heap/new machinery; host OOM is not a guest NULL/handler call.
    tail.soundAllocationBytes=0x250u;tail.soundAllocationEntered=true;
    using G=assets::OriginalConfigProcessGlobals50LA;
    if(!G::reentrancyCurrentNative()||G::state().reent38544C!=G::StdioIdentity::reent385160)
        return stop(0x30570c,"Source sound allocation process custody unavailable");
    tail.soundAllocationReentRead=true;
    try {tail.firstSound=std::make_unique<original_race_music_source50LA::Sound>();}
    catch(...){return stop(0x305990,"Native sound backing unavailable; guest allocation result unknown");}
    auto& sound=*tail.firstSound;auto& e=sound.entity;sound.nativeIssuer=this;
    tail.soundAllocationReturn=&e;tail.soundAllocationReturned=true;
    sound.constructorEntered=true; //1A3CBC a1=1;1A3CC0 ->2C20F0, nonnull allocation.
    //2C210C/10/14 select SAME Application28C Race110 and Application00 Aural.
    //The source group's native lifetime is already pinned by this manager.
    auto& groups=*p.sourceGroups;
    if(!groups.coldStarted_||groups.coldRetiring_||groups.coldThread_!=p.thread||
       groups.coldApplication_!=p.application||groups.coldSession_!=p.component.sessionGeneration||
       race.raceGroupCount_!=6u||race.raceGroupsAttached_!=6u)
        return stop(0x2c2114,"Actual Effects110 group owner unavailable");
    auto& effects=groups.coldGroups_[static_cast<std::size_t>(RaceAudioGroup50IC::Effects110)];
    if(race.raceGroups_[0]!=&effects.entity||!effects.constructed||!effects.loaded||
       !effects.enabled30||!effects.allocated58||!effects.logicalMembers4C||
       effects.entity.parent1C!=&race.entity_||effects.entity.reference00!=1u||
       effects.entity.kind!=OriginalWreckComponentKind50LA::entity141300||
       effects.entity.sessionGeneration!=p.component.sessionGeneration||
       !p.application->entityAuthority_.canRetire(effects.entity)||effects.slot40<0||
       std::size_t(effects.slot40)>=p.application->soundGroups434A10_.size()||
       p.application->soundGroups434A10_[effects.slot40]!=&effects.entity)
        return stop(0x2c2114,"Actual Race110/global Effects group identity unavailable");
    if(!p.aural||!p.aural->current()||p.aural->issuer_!=&app||p.aural->entities_!=p.application||
       p.application->auralNativeOwner_!=p.aural.get()||!p.application->aural00Live_||
       p.aural->inspect().entity!=p.application->aural00_)
        return stop(0x2c2110,"Actual same-Application Aural Entity unavailable");
    sound.emitterEntered=true; //2C2118 ->2928B0 ->290F10, type a3=1 and saved t0=1.
    const auto key=std::uint64_t(reinterpret_cast<std::uintptr_t>(&e));
    if(p.application->objectCount423F60()==std::numeric_limits<std::uint32_t>::max()||
       !p.application->entityAuthority_.reserveAdditional(1)||!p.application->entityAuthority_.canEnroll(e,key))
        return stop(0x290f38,"Source emitter count/enrollment capacity unavailable");
    const auto made=initializeOriginalWreckComponent50LA(e,OriginalWreckComponentKind50LA::soundEmitter2C20F0,
        p.component.sessionGeneration,key,true,p.application->objectCount423F60());
    if(made.status!=OriginalWreckComponentStatus50LA::complete||
       !p.application->entityAuthority_.enroll(e))std::terminate();
    sound.counted=true;sound.dispatch08=0x3d34f0u; //290F4C after141300 returned.
    constexpr std::array<std::uint32_t,4> vf0{{0,0,0,0x3f800000u}};
    for(std::size_t i=0;i<4;++i)sound.previous40[i]=vf0[i]; //290F58 SQC2 vf0.
    for(const auto at:{4u,8u,16u})for(std::size_t i=0;i<4;++i)
        sound.configuration160[at+i]=vf0[i]; //290F64/70/7C.
    auto& shared=p.application->garageEmitterShared_;sound.sharedEmitterIdentity=&shared;
    //Same retained emitter owner as Garage/TA-intro/Player speech. Existing
    //names abbreviate full VAs4434C0/4434B8/443470; no per-Music state/reset.
    if(!shared.guard8C5C){shared.assertion434C0=1u;shared.guard8C5C=1u;}
    if(shared.assertion434C0){
        sound.assertionEntered=true;
        //Actual retained nonnull Aural argument makes133660 return0.
        sound.assertionReturn=0u;
    }
    for(auto& word:sound.configuration160)word=0u; //290FF4 memset160..1CF.
    sound.manager1D8=p.application->aural00_; //291014: Entity, NOT interfaceA0/mixer.
    sound.group30=&effects.entity; //291020: actual Effects110 source group.
    sound.sound1F0=nullptr; //291028.
    constexpr std::array<std::uint32_t,4> sentinel{{0x7f7fffffu,0x7f7fffffu,0x7f7fffffu,0x3f800000u}};
    for(std::size_t i=0;i<4;++i)sound.previous40[i]=sentinel[i]; //291044.
    for(std::size_t i=0;i<4;++i)sound.configuration160[4+i]=sentinel[i]; //291050.
    sound.flags158=0u;sound.trigger1DC=0u;sound.chance1E0=0x42c80000u; //291058/60/68.
    //309230 of exact empty literal3BD260. One-byte native string-copy
    //adaptation; no initialization of the remaining source name54 tail.
    sound.nameFirst54=0u; //291070/74.
    sound.type1E4=1u; //29108C, original constructor type argument.
    for(const auto bit:{1u,2u,4u,8u,16u}){
        sound.flags1D4KnownValue&=static_cast<std::uint8_t>(~bit);
        sound.flags1D4KnownMask|=static_cast<std::uint8_t>(bit); //Five original stores, upper3 bits UNKNOWN.
    }
    sound.serial50=shared.serial434B8; //291120 historical value.
    ++shared.serial434B8; //291134 wrapping32-bit shared write; never rollback on a later bound.
    sound.distance1D0=0u;sound.word1E8=0u;sound.word1EC=1u; //291138/144/14C.
    for(std::size_t i=0;i<4;++i)sound.previous40[i]=std::bit_cast<std::uint32_t>(shared.origin43470[i]); //291158.
    sound.emitterReturn=&e;sound.emitterReturned=true; //29115C/170 actual local290F10 return.
    sound.dispatch08=0x3d32c0u; //2928D0, intermediate derived constructor.
    const auto constructVector=[](auto& vector)noexcept {
        vector.constructorEntered=true;
        vector.backing04=nullptr;vector.count00=0u;vector.growth08=0u;vector.capacity0C=0u;
        vector.flags10KnownValue&=static_cast<std::uint8_t>(~1u);vector.flags10KnownMask|=1u;
        vector.returnedThis=&vector;vector.constructorReturned=true;
    };
    constructVector(sound.vector200); //2928D8 ->293CE0; bit store in293D0C return delay slot.
    constructVector(sound.vector214); //2928E4 ->293960;29398C delay store.
    constructVector(sound.vector228); //2928F0 ->293930;29395C delay store.
    sound.initializeReceiver=&sound.vector200;sound.initializeInitial=1u;sound.initializeGrowth=16u;
    sound.initialize293D10Entered=true; //292904 call; first unresolved read is293D24.
    return stop(0x293d24,"Original vector guard8C70 process history is unowned; constructor continuation remains bounded");
    //No guard/assertion seed,4-byte vector allocation,293D10/2928B0/2C20F0
    //return,C0[0] publication,2C21C0 setup,loop increment,listener or playback.
}

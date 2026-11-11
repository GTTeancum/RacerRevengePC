#pragma once
// PRIVATE, UNACTIVATED SOURCE-ONLY CANDIDATE. Never compiled or executed.
#include <cstring>
#include <utility>
namespace rr::platform {
inline bool RaceAudioGroups50IC::coldTaIntroGroupIdentity()const noexcept {
    if(!coldStarted_||!coldApplication_||!coldMixer_||
       coldThread_!=std::this_thread::get_id()||coldApplication_->nativeOwnerThread_!=coldThread_||
       !coldApplication_->application10()||coldApplication_->attachmentLock423F58()||
       coldEpoch_!=coldMixer_->groupEpoch50IC())return false;
    const auto& p=coldGroups_[static_cast<std::size_t>(game::RaceAudioGroup50IC::Announcer120)];
    const auto& e=p.entity;
    if(!p.constructed||!p.loaded||!p.enabled30||!p.allocated58||!p.logicalMembers4C||
       p.capacity54!=8||p.growth50!=16||p.count48>p.taIntroSounds.size()||e.reference00!=1||
       e.firstChild18||e.descendants24||e.sessionGeneration!=coldSession_||
       e.objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&e))||
       p.slot40<0||static_cast<std::size_t>(p.slot40)>=coldApplication_->soundGroups434A10_.size()||
       coldApplication_->soundGroups434A10_[p.slot40]!=&e||
       !coldApplication_->entityAuthority_.canRetire(e))return false;
    return game::original_entity_chain_detail50LA::forest(coldApplication_->entityAuthority_.view());
}
inline bool RaceAudioGroups50IC::coldTaIntroSoundIdentity(std::size_t n)const noexcept {
    const auto& p=coldGroups_[static_cast<std::size_t>(game::RaceAudioGroup50IC::Announcer120)];
    if(n>=p.taIntroSounds.size()||n>=p.count48||!p.taIntroStarted||!p.taIntroAuthoring||!p.taIntroManager||
       !p.taIntroSounds[n]||p.logicalMembers4C[n]!=p.taIntroSounds[n].get())return false;
    const auto& s=*p.taIntroSounds[n];const auto* event=p.taIntroAuthoring->event(static_cast<unsigned>(n+1));
    const auto observation=p.taIntroManager->inspect();
    if(!event||!event->metadata||!observation.interfaceA0||
       coldApplication_->auralNativeOwner_!=p.taIntroManager.get()||!coldApplication_->aural00Live_||
       coldApplication_->aural00_!=observation.entity||s.manager30!=observation.interfaceA0||
       s.group34!=&p||s.reference00!=1||s.flags10!=2||s.word14||s.word18||
       s.sample28!=-1||(!taIntroLive_&&(s.channel2C!=-1||s.elapsedBits24))||s.resourceA0!=-1||
       !s.stream1A8||s.stereo1AC||s.loop1B0||s.word1B4||
       s.metadata!=event->metadata||s.directoryOrdinal1B8!=static_cast<std::int32_t>(event->metadata->sourceIndex)||
       s.duration20!=event->metadata->admittedDurationSeconds||
       s.basePitch50!=static_cast<std::int32_t>(event->metadata->originalPitch)||
       // v5 permits only the four real setters' bounded mutable fields. The
       // original metadata pitch50 and every never-started identity stay fixed.
       (s.pitch40!=s.basePitch50&&static_cast<std::uint32_t>(s.pitch40)>=16384u)||
       static_cast<std::uint32_t>(s.pitch4C)>=16384u||
       static_cast<std::uint32_t>(s.volume38)>100u||
       static_cast<std::uint32_t>(s.volume44)>100u||s.pan3C||s.pan48||
       s.factor54Bits!=0x3f800000u||s.factor58Bits!=0x3f800000u||s.factor5CBits!=0x3e800000u||
       s.word70||s.word74||s.word78||(!taIntroLive_&&s.word7C)||s.word7C>1||s.word80||
       event->filename.empty()||event->filename.size()>259||
       std::memcmp(s.filenameA4.data(),event->filename.data(),event->filename.size())||
       s.filenameA4[event->filename.size()]!='\0')return false;
    constexpr std::array<std::uint32_t,4> identity{{0,0,0,0x3f800000u}};
    return s.vector60Bits==identity&&s.vector90Bits==identity;
}
inline bool RaceAudioGroups50IC::prepareColdTaIntroConstruction(
    const std::shared_ptr<const assets::OriginalTaIntroAuthoringCandidate>& authoring,
    std::shared_ptr<const OriginalApplicationAural50KG>& manager)const noexcept {
    if(!current()||!groups_.initialized||!coldTaIntroGroupIdentity()||
       coldNext_!=game::raceAudioGroupCount50IC||!authoring||!authoring->eventCount()||authoring->eventCount()>3||(authoring->trackId()!="TA"&&authoring->trackId()!="RA"&&authoring->trackId()!="GC"&&authoring->trackId()!="SA"&&authoring->trackId()!="MA"&&authoring->trackId()!="GA"&&authoring->trackId()!="MB"&&authoring->trackId()!="SC"&&authoring->trackId()!="GB")||
       !authoring->providerOwner()||!authoring->metadataOwner()||
       !authoring->providerOwner()->sealed()||
       authoring->providerOwner()->revision()!=authoring->capturedProviderRevision())return false;
    const auto& p=coldGroups_[static_cast<std::size_t>(game::RaceAudioGroup50IC::Announcer120)];
    if(p.count48||p.taIntroStarted||p.taIntroConstructed||p.taIntroAuthoring||p.taIntroManager)return false;
    for(const auto& sound:p.taIntroSounds)if(sound)return false;
    if(taIntroConstructionAdmission_){
        if(!taIntroExistingBindingsCurrent())return false;
    }else for(std::size_t n=0;n<count_;++n)
        if(mixer_->active(n)||mixer_->hasVoiceGroupGain50IC(n)||
           bindings_[n]!=game::RaceAudioGroup50IC::Unrepresented)return false;
    const auto* actualAural=coldApplication_->auralNativeOwner_;
    if(!actualAural||!coldApplication_->aural00Live_)return false;
    auto retained=actualAural->retainColdStreamManager232DB0(coldApplication_,
        authoring->metadataOwner(),authoring->providerOwner());
    if(!retained)return false;
    const auto observation=retained->inspect();
    if(!observation.interfaceA0||observation.entity!=coldApplication_->aural00_)return false;
    for(unsigned id=1;id<=authoring->eventCount();++id){
        const auto* event=authoring->event(id);
        if(!event||event->eventId!=id||event->factoryFlags!=2||!event->isStream||
           event->isStereo||event->isLoop||!event->metadata||event->filename.empty()||
           event->filename.size()>259||event->filename.find('\0')!=std::string_view::npos||
           !event->filename.ends_with(".vag")||
           authoring->metadataOwner()->findForStreamRequest(true,event->filename)!=event->metadata||
           event->metadata->sourceIndex>static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max())||
           event->metadata->originalPitch>static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max()))return false;
    }
    manager=std::move(retained);return true;
}
inline RaceAudioGroups50IC::ColdSound* RaceAudioGroups50IC::appendPreparedColdTaIntroSound232ED0(
    std::unique_ptr<ColdSound> reserved)noexcept {
    auto& p=coldGroups_[static_cast<std::size_t>(game::RaceAudioGroup50IC::Announcer120)];
    const auto n=p.count48;const auto observation=p.taIntroManager->inspect();
    // Private nonfailing source commit, only after exact caller preflight and
    // native allocation. No callback/reentry can intervene on this owner thread.
    auto& s=*reserved;const auto& event=*p.taIntroAuthoring->event(static_cast<unsigned>(n+1));
    s.reference00=1; //1027B0 ->1048F0. NOT an Entity/count contribution.
    s.flags10=0;s.word14=0;s.word18=0;
    s.vector60Bits={{0,0,0,0x3f800000u}}; //232F0C vf0.
    s.vector90Bits={{0,0,0,0x3f800000u}}; //232F18 vf0.
    s.sample28=-1;s.channel2C=-1;s.manager30=observation.interfaceA0;s.group34=&p;
    //235270 is reached before the remaining232ED0 defaults. The actual
    //same-Application assertion state is process history, never reset here.
    if(!coldApplication_->soundInsertGuard881C_){
        coldApplication_->soundInsertAssertion434A88_=1;
        coldApplication_->soundInsertGuard881C_=1;
    }
    //235A60 appends THIS stable pointer without a reference increment.
    p.taIntroSounds[n]=std::move(reserved);
    p.logicalMembers4C[p.count48]=&s; //235B90 pointer precedes count.
    ++p.count48; //235B9C.
    s.volume38=100;s.pan3C=0;s.pitch40=4096;
    s.volume44=100;s.pan48=0;s.pitch4C=4096;s.basePitch50=4096;
    s.factor54Bits=0x3f800000u;s.factor58Bits=0x3f800000u;s.factor5CBits=0x3e800000u;
    s.word78=0;
    //4349D0's actual startup3CA250..3CA2AC value. Unknown84/88/8C absent.
    s.vector60Bits={{0,0,0,0x3f800000u}};
    s.word70=0;s.word74=0;s.word7C=0;s.word80=0;
    s.vector90Bits={{0,0,0,0x3f800000u}};
    s.stream1A8=false;s.stereo1AC=false;s.loop1B0=false;s.word1B4=0;
    s.elapsedBits24=0;s.duration20=0.f;s.directoryOrdinal1B8=-1;s.resourceA0=-1;
    s.filenameA4[0]='\0';
    // Bounded233110 mono-stream metadata projection. No source backend
    // initialized/RPC assertion parity or hardware conversion is claimed.
    s.flags10=event.factoryFlags;s.stream1A8=true;
    std::memcpy(s.filenameA4.data(),event.filename.data(),event.filename.size());
    s.filenameA4[event.filename.size()]='\0'; //Already genuine .vag spelling.
    s.metadata=event.metadata;s.directoryOrdinal1B8=static_cast<std::int32_t>(s.metadata->sourceIndex);
    s.duration20=s.metadata->admittedDurationSeconds;
    s.basePitch50=static_cast<std::int32_t>(s.metadata->originalPitch); //233248.
    s.pitch40=s.basePitch50; //233254, after50.
    //233254 changes40 only:4C remains constructor4096. No sample/channel.
    //232E5C..68: re-copy the original parsed name only AFTER233110,
    //with strncpy's259-byte bound/padding, then terminate source1A7.
    //The immutable view is not NUL-terminated, so copy its exact parsed
    //length and explicitly pad rather than reading beyond the name slice.
    std::memcpy(s.filenameA4.data(),event.filename.data(),event.filename.size());
    for(std::size_t byte=event.filename.size();byte<259;++byte)s.filenameA4[byte]='\0';
    s.filenameA4[259]='\0';
    return &s;
}
inline bool RaceAudioGroups50IC::constructColdTaIntroSounds232ED0(
    std::shared_ptr<const assets::OriginalTaIntroAuthoringCandidate> authoring)noexcept {
    std::shared_ptr<const OriginalApplicationAural50KG> manager;
    if(!prepareColdTaIntroConstruction(authoring,manager))return false;
    std::array<std::unique_ptr<ColdSound>,3> reserved;
    try{for(std::size_t n=0;n<authoring->eventCount();++n)reserved[n]=std::make_unique<ColdSound>();}catch(...){return false;}
    auto& p=coldGroups_[static_cast<std::size_t>(game::RaceAudioGroup50IC::Announcer120)];
    p.taIntroAuthoring=std::move(authoring);p.taIntroManager=std::move(manager);p.taIntroStarted=true;
    for(std::size_t n=0;n<p.taIntroAuthoring->eventCount();++n)appendPreparedColdTaIntroSound232ED0(std::move(reserved[n]));
    p.taIntroConstructed=true;return true;
}
inline bool RaceAudioGroups50IC::clearColdTaIntroSounds2353D0()noexcept {
    if(coldTaIntroEmitter_||!coldTaIntroGroupIdentity())return false;
    auto& p=coldGroups_[static_cast<std::size_t>(game::RaceAudioGroup50IC::Announcer120)];
    if(!p.taIntroStarted||!p.taIntroAuthoring||!p.taIntroManager||p.count48>p.taIntroSounds.size())return false;
    for(std::size_t n=0;n<p.count48;++n)if(!coldTaIntroSoundIdentity(n))return false;
    std::unique_ptr<ColdSound*[]> snapshot;
    try{if(p.count48)snapshot=std::make_unique<ColdSound*[]>(p.count48);}catch(...){return false;}
    for(std::size_t n=0;n<p.count48;++n)snapshot[n]=p.logicalMembers4C[n];
    const auto oldCount=p.count48;
    for(std::size_t n=0;n<oldCount;++n){
        auto& s=*snapshot[n];
        //235464 virtual+20(a1=0) ->233670: exactly channel==-1 returns1,
        //with no24/2C/7C writes and no backend call. Preflight proved this arm.
        //235474 virtual+10 ->104970. Ref1 reaches0;233070 flags2/channel<0
        //skips26ABD0, then1027F0/104910 frees this non-Entity Sound.
        --s.reference00;p.taIntroSounds[n].reset();
    }
    snapshot.reset(); //235490 temporary backing free precedes235C40.
    for(std::size_t n=0;n<oldCount;++n)p.logicalMembers4C[n]=nullptr;
    p.count48=0; //235C64 only after every ordered stop/release.
    p.taIntroAuthoring.reset();p.taIntroManager.reset();
    // Keep original capacity/growth/allocated backing and process guard history.
    // Existing releaseEmptyCold142590 remains the only group retirement path.
    return true;
}
} // namespace rr::platform

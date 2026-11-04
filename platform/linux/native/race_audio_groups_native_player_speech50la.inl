#pragma once
// Private source-only constructor continuation. No event loading, dispatch,
// playback, RNG draw, source attachment or full22A210 return is implemented.
#include <bit>
#include <limits>
namespace rr::platform {
inline bool RaceAudioGroups50IC::nativePlayerSpeechContextCurrent(
    const std::shared_ptr<game::OriginalApplicationEntities50LA>& app,std::uint64_t generation,
    const game::OriginalWreckComponent50LA& race,const game::OriginalWreckComponent50LA& character,
    const game::OriginalWreckComponent50LA& racer)const noexcept {
    if(!app||coldApplication_!=app||app.owner_before(coldApplication_)||coldApplication_.owner_before(app)||
       !generation||generation!=coldSession_||!coldStarted_||coldRetiring_||nativeBindingsRetired_||
       coldThread_!=std::this_thread::get_id()||app->nativeOwnerThread_!=coldThread_||
       !app->application10()||app->attachmentLock423F58()||!groups_.initialized||
       coldNext_!=game::raceAudioGroupCount50IC||!coldMixer_||coldMixer_!=mixer_||
       coldEpoch_!=epoch_||mixer_->groupEpoch50IC()!=epoch_||mixer_->count()!=count_||
       !mixer_->requiresVoiceGroupGains50IC())return false;
    const auto& p=coldGroups_[static_cast<std::size_t>(game::RaceAudioGroup50IC::Character11C)];
    if(&p.entity!=&character||!p.constructed||!p.loaded||!p.enabled30||!p.allocated58||
       !p.logicalMembers4C||p.count48||p.capacity54!=8u||p.growth50!=16u||
       p.taIntroAuthoring||p.taIntroManager||p.taIntroStarted||p.taIntroConstructed||
       character.parent1C!=&race||character.firstChild18||character.descendants24||
       race.sessionGeneration!=generation||character.sessionGeneration!=generation||
       racer.sessionGeneration!=generation||racer.reference00!=1u||!racer.objectKey||
       racer.kind!=game::OriginalWreckComponentKind50LA::racer27E130||
       p.slot40<0||std::size_t(p.slot40)>=app->soundGroups434A10_.size()||
       app->soundGroups434A10_[p.slot40]!=&character||
       !app->entityAuthority_.canRetire(race)||!app->entityAuthority_.canRetire(character))return false;
    // Racer belongs to Session's genuine counted cold domain, NOT this
    // Application registry. Private Bridge custody proves the exact Session,
    // model, key and shared counter/lock before entering this leaf. Do not
    // retrospectively enroll it or conflate the two existing ownership domains.
    for(const auto& sound:p.taIntroSounds)if(sound)return false;
    // SAME Application00 Aural, not a native mixer or interface substituted
    // for source manager1D8. Existing live group slots bar Aural retirement.
    const auto* aural=app->auralNativeOwner_;
    if(!aural||!app->aural00Live_||!aural->current()||
       aural->entities_!=app||aural->entities_.owner_before(app)||app.owner_before(aural->entities_)||
       aural->inspect().entity!=app->aural00_)return false;
    // This checks only enrolled Application owners. The new D48 is detached
    // and has no source link into the Session racer domain; the forest is not
    // evidence of racer enrollment, attachment or future22A25C completion.
    return game::original_entity_chain_detail50LA::forest(app->entityAuthority_.view());
}
inline NativePlayerSpeechConstructorResult50LA RaceAudioGroups50IC::constructNativePlayerSpeech2C1000(
    const void* issuer,const std::shared_ptr<game::OriginalApplicationEntities50LA>& app,std::uint64_t generation,
    const game::OriginalWreckComponent50LA& race,const game::OriginalWreckComponent50LA& character,
    game::OriginalWreckComponent50LA& racer,game::PodSpeechCounts50HG& counts,game::PodSpeechState50HG& state)noexcept {
    using S=NativePlayerSpeechConstructorStatus50LA;
    if(!issuer||nativePlayerSpeechAttempted_||coldNativePlayerSpeech_||state.initialized||
       !nativePlayerSpeechContextCurrent(app,generation,race,character,racer)||
       app->objectCount423F60()==std::numeric_limits<std::uint32_t>::max())return {};
    nativePlayerSpeechAttempted_=true;
    std::unique_ptr<ColdNativePlayerSpeech> next;
    // Native backing for the actual22A220 requested0x230 source object. This
    // typed host projection is not a guest byte allocation/OOM simulation.
    // A refusal stops BEFORE141300/count/serial, never fabricates return0.
    try{next=std::make_unique<ColdNativePlayerSpeech>();}
    catch(...){return {S::nativeAllocationPending,0x22a220u,nullptr};}
    const auto key=std::uint64_t(reinterpret_cast<std::uintptr_t>(&next->entity));
    if(!app->entityAuthority_.reserveAdditional(1))
        return {S::nativeAllocationPending,0x22a220u,nullptr};
    if(!app->entityAuthority_.canEnroll(next->entity,key))return {};
    auto& e=*next;
    e.nativeIssuer=issuer;e.nativeAural=app->auralNativeOwner_;
    e.nativeRace=&race;e.nativeExpectedRacer=&racer;e.nativeExpectedRacerKey=racer.objectKey;
    e.counts214=&counts;e.state21D=&state;
    const auto made=game::initializeOriginalWreckComponent50LA(e.entity,
        game::OriginalWreckComponentKind50LA::speech2C1000,generation,key,true,app->objectCount423F60());
    if(made.status!=game::OriginalWreckComponentStatus50LA::complete||
       !app->entityAuthority_.enroll(e.entity))std::terminate();
    e.dispatch08=0x3d34f0u; //290F4C, base Emitter vtable.
    constexpr std::array<std::uint32_t,4> vf0{{0,0,0,0x3f800000u}};
    e.previous40=vf0;
    for(std::size_t n=0;n<4;++n)e.configuration160[4+n]=vf0[n];
    for(std::size_t n=0;n<4;++n)e.configuration160[8+n]=vf0[n];
    for(std::size_t n=0;n<4;++n)e.configuration160[16+n]=vf0[n];
    auto& shared=app->garageEmitterShared_;
    if(!shared.guard8C5C){shared.assertion434C0=1u;shared.guard8C5C=1u;}
    // The same live nonnull Aural proves the reached290FAC assertion arm.
    for(auto& word:e.configuration160)word=0; //290FF4 memset160..1CF.
    e.manager1D8=app->aural00_;
    e.group30=&coldGroups_[static_cast<std::size_t>(game::RaceAudioGroup50IC::Character11C)];
    e.sound1F0=nullptr;
    e.previous40={{0x7f7fffffu,0x7f7fffffu,0x7f7fffffu,0x3f800000u}};
    for(std::size_t n=0;n<4;++n)e.configuration160[4+n]=e.previous40[n];
    e.flags158=0;e.trigger1DCBits=0;e.chance1E0Bits=0x42c80000u;e.name54='\0';
    e.type1E4=1u; //Actual2C1018 argument, not2921C0's type6.
    // These five stores preserve source upper bits by leaving them UNKNOWN
    // in the projection, not zero-filling an invented whole1D4 byte.
    e.flags1D4Low5&=static_cast<std::uint8_t>(~1u);
    e.flags1D4Low5&=static_cast<std::uint8_t>(~2u);
    e.flags1D4Low5&=static_cast<std::uint8_t>(~4u);
    e.flags1D4Low5&=static_cast<std::uint8_t>(~8u);
    e.flags1D4Low5&=static_cast<std::uint8_t>(~16u);
    e.serial50=shared.serial434B8;++shared.serial434B8; //291120 then291134, wrapping.
    e.distance1D0Bits=0;e.word1E8=0;e.word1EC=1;
    for(std::size_t n=0;n<4;++n)e.previous40[n]=std::bit_cast<std::uint32_t>(shared.origin43470[n]);
    e.dispatch08=0x3d4c90u; //2C1038, AFTER290F10 returned.
    e.jukebox200[0]=nullptr;e.jukebox200[1]=nullptr;e.jukebox200[2]=nullptr;e.racer20C=nullptr;
    // EXACT214..222 source order in the SAME fields consumed downstream.
    counts.lowDamage=0;counts.wallHit=0;counts.passed=0;counts.attacked=0;
    counts.passing=0;counts.attacking=0;counts.death=0;counts.win=0;counts.lose=0;
    for(auto& cursor:state.cursors)cursor=0;
    state.cooldown224=5.f; //2C108C. No write to210, RNG or native initialized.
    coldNativePlayerSpeech_=std::move(next);
    return {S::constructorComplete,0x2c10a0u,&coldNativePlayerSpeech_->entity};
}
inline bool RaceAudioGroups50IC::nativePlayerSpeechBackingCurrent()const noexcept {
    if(!nativePlayerSpeechAttempted_||!coldNativePlayerSpeech_)return false;
    const auto& e=*coldNativePlayerSpeech_;const auto& n=e.entity;
    if(!e.nativeIssuer||!e.nativeRace||!e.nativeExpectedRacer||!e.nativeExpectedRacerKey||
       e.nativeExpectedRacer->objectKey!=e.nativeExpectedRacerKey||!e.group30||
       !e.counts214||!e.state21D||!nativePlayerSpeechContextCurrent(coldApplication_,coldSession_,
            *e.nativeRace,e.group30->entity,*e.nativeExpectedRacer)||
       e.nativeAural!=coldApplication_->auralNativeOwner_||e.manager1D8!=coldApplication_->aural00_||
       n.kind!=game::OriginalWreckComponentKind50LA::speech2C1000||n.reference00!=1u||
       n.sessionGeneration!=coldSession_||n.objectKey!=std::uint64_t(reinterpret_cast<std::uintptr_t>(&n))||
       !coldApplication_->entityAuthority_.canRetire(n)||n.previous10||n.next14||n.firstChild18||n.parent1C||
       n.mask20||n.descendants24||n.category28!=0xffu||(n.flags29&15u)!=3u||n.nativeFlags29||
       e.dispatch08!=0x3d4c90u||e.flags158||e.flags1D4Low5||e.trigger1DCBits||
       e.chance1E0Bits!=0x42c80000u||e.type1E4!=1u||e.name54||e.distance1D0Bits||
       e.word1E8||e.word1EC!=1u||e.sound1F0||e.racer20C||e.timing210)return false;
    for(const auto* child:e.jukebox200)if(child)return false;
    // Numeric state is Bank-restored storage. Global group current()/uses()
    // must never read whichever racer's bank slot currently occupies it.
    // Constructor and retirement validate/write it only in the human slot0.
    for(std::size_t i=0;i<4;++i)
        if(e.previous40[i]!=std::bit_cast<std::uint32_t>(coldApplication_->garageEmitterShared_.origin43470[i]))return false;
    for(std::size_t i=0;i<e.configuration160.size();++i){
        const auto expected=(i>=4&&i<7)?0x7f7fffffu:i==7?0x3f800000u:0u;
        if(e.configuration160[i]!=expected)return false;
    }
    // serial50 is historical. Later genuine emitters may advance the same
    // process serial, so global-1 is deliberately NOT an identity condition.
    return true;
}
inline bool RaceAudioGroups50IC::canRetireNativePlayerSpeech2C10B0(const void* issuer,
    const game::OriginalWreckComponent50LA& racer,const game::OriginalWreckComponent50LA* published)const noexcept {
    if(!nativePlayerSpeechBackingCurrent()||!coldApplication_->objectCount423F60()||
       coldNativePlayerSpeech_->nativeIssuer!=issuer||coldNativePlayerSpeech_->nativeExpectedRacer!=&racer||
       published!=&coldNativePlayerSpeech_->entity)return false;
    // Only Bridge's authenticated Bank.visit(0) may call this private leaf.
    // Never use these values as global Group identity or an initialized receipt.
    const auto& c=*coldNativePlayerSpeech_->counts214;const auto& state=*coldNativePlayerSpeech_->state21D;
    if(c.lowDamage||c.wallHit||c.passed||c.attacked||c.passing||c.attacking||c.death||c.win||c.lose||
       state.initialized||std::bit_cast<std::uint32_t>(state.cooldown224)!=0x40a00000u)return false;
    for(const auto cursor:state.cursors)if(cursor)return false;
    return true;
}
inline bool RaceAudioGroups50IC::retireNativePlayerSpeech2C10B0(const void* issuer,
    const game::OriginalWreckComponent50LA& racer,const game::OriginalWreckComponent50LA* historical)noexcept {
    if(!canRetireNativePlayerSpeech2C10B0(issuer,racer,historical))return false;
    auto& e=*coldNativePlayerSpeech_;
    // Caller has withdrawn D48/return aliases; liveAudio has revoked its
    // forward borrows. This never-attached, no-child142590 release reaches
    //104970 then2C10B0. The three actual null children skip virtual releases.
    if(!coldApplication_->entityAuthority_.retire(e.entity))return false;
    --e.entity.reference00;
    e.dispatch08=0x3d4c90u; //2C10D4.
    e.dispatch08=0x3d34f0u; //2911A4 after the null200/204/208 tests.
    --coldApplication_->objectCount423F60(); //2911A8 ->1413E0.
    e.counts214=nullptr;e.state21D=nullptr;e.nativeIssuer=nullptr;
    e.nativeExpectedRacer=nullptr;e.nativeExpectedRacerKey=0;e.nativeRace=nullptr;e.nativeAural=nullptr;
    coldNativePlayerSpeech_.reset(); //Native backing released after source count.
    return true;
}
} // namespace rr::platform

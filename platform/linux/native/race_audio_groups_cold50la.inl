#pragma once
#include <limits>
namespace rr::platform {
inline bool RaceAudioGroups50IC::constructNextCold2350D0(AudioMixer50HD& mixer,
    std::shared_ptr<game::OriginalApplicationEntities50LA> app,std::uint64_t session,
    game::RaceAudioGroup50IC group)noexcept {
    using G=game::RaceAudioGroup50IC;
    constexpr G order[]={G::Effects110,G::Engines114,G::Music118,G::Character11C,
        G::Announcer120,G::Auxiliary124,G::World4D8};
    if(coldRetiring_||!app||!app->application10()||!session||mixer_||groups_.initialized||
       coldNext_>=game::raceAudioGroupCount50IC||group!=order[coldNext_]||
       !mixer.count()||mixer.requiresVoiceGroupGains50IC()||app->attachmentLock423F58())return false;
    if(coldStarted_&&(coldApplication_!=app||coldMixer_!=&mixer||coldSession_!=session||
       coldEpoch_!=mixer.groupEpoch50IC()||coldThread_!=std::this_thread::get_id()))return false;
    for(std::size_t i=0;i<mixer.count();++i)
        if(mixer.active(i)||mixer.hasVoiceGroupGain50IC(i))return false;
    const auto index=static_cast<std::size_t>(group);auto& p=coldGroups_[index];
    const auto key=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&p.entity));
    if(p.constructed||!app->entityAuthority_.canEnroll(p.entity,key)||
       app->objectCount423F60()==std::numeric_limits<std::uint32_t>::max())return false;
    // Native backing is reserved before any source reference/count effect.
    //235900 allocates8 pointers; no element is read while actual count48 is0.
    std::unique_ptr<game::OriginalWreckComponent50LA*[]> storage;
    try {storage.reset(new game::OriginalWreckComponent50LA*[8]);}catch(...){return false;}
    std::size_t slot=0;
    while(slot<app->soundGroups434A10_.size()&&app->soundGroups434A10_[slot])++slot;
    if(slot==app->soundGroups434A10_.size())return false; //No invalid-1 owner admitted.
    if(!coldStarted_){coldApplication_=app;coldMixer_=&mixer;coldSession_=session;
        coldEpoch_=mixer.groupEpoch50IC();coldThread_=std::this_thread::get_id();coldStarted_=true;}
    const auto r=game::initializeOriginalWreckComponent50LA(p.entity,
        game::OriginalWreckComponentKind50LA::entity141300,session,key,true,app->objectCount423F60());
    if(r.status!=game::OriginalWreckComponentStatus50LA::complete)std::terminate();
    if(!app->entityAuthority_.enroll(p.entity))std::terminate();
    p.constructed=true;p.enabled30=true; //102620, with actual A1=1 via2350D0.
    groups_.volume38[index]=0; //10265C. SAME values consumed by mixer callbacks.
    p.count48=0;p.growth50=16;p.capacity54=8;
    p.logicalMembers4C=std::move(storage);p.allocated58=true;
    p.slot40=2;groups_.state44[index]=0; //235128..138, before virtual30.
    //2351F0: genuine process-wide first-free allocation, not enum-as-ID.
    app->soundGroups434A10_[slot]=&p.entity;
    p.slot40=static_cast<std::int32_t>(slot);groups_.volume38[index]=100;p.loaded=true;
    ++coldNext_;return true;
}
inline bool RaceAudioGroups50IC::releaseEmptyCold142590(game::RaceAudioGroup50IC group)noexcept {
    if(!represented(group)||!coldStarted_||!coldApplication_||!coldMixer_||
       coldThread_!=std::this_thread::get_id()||coldEpoch_!=coldMixer_->groupEpoch50IC()||
       coldApplication_->attachmentLock423F58())return false;
    auto& p=coldGroups_[static_cast<std::size_t>(group)];auto& e=p.entity;
    if(!p.constructed||!p.loaded||p.count48||!p.logicalMembers4C||
       e.reference00!=1||e.previous10||e.next14||e.parent1C||e.firstChild18||
       e.sessionGeneration!=coldSession_||e.objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&e))||
       p.slot40<0||static_cast<std::size_t>(p.slot40)>=coldApplication_->soundGroups434A10_.size()||
       coldApplication_->soundGroups434A10_[p.slot40]!=&e||!coldApplication_->objectCount423F60())return false;
    // No provider binding is admitted on this route, even after final levels.
    for(std::size_t i=0;i<coldMixer_->count();++i)
        if(coldMixer_->active(i)||coldMixer_->hasVoiceGroupGain50IC(i)||
           bindings_[i]!=game::RaceAudioGroup50IC::Unrepresented)return false;
    if(!coldApplication_->entityAuthority_.retire(e))return false;
    coldRetiring_=true; // A partially retired group set cannot resume loading.
    --e.reference00; //142590/104970 -> concrete235150.
    coldApplication_->soundGroups434A10_[p.slot40]=nullptr; //23518C precedes2353D0.
    // The real list has always had count0:2353E8 skips snapshot/callback loop.
    // No voice state is substituted for a logical member callback.
    p.logicalMembers4C.reset(); //2351AC..B0, after the empty typed clear.
    --coldApplication_->objectCount423F60(); //102670 ->1413E0.
    p.constructed=false;p.loaded=false;return true;
}
}

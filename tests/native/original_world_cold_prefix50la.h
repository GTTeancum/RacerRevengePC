#pragma once
#include "original_ui_main_entry50kg.h"
#include "ra_cold_aggregate50la.h"
#include <limits>
namespace ra_pod_scene50LA {
inline bool Owner::constructWorldColdPrefix(std::shared_ptr<g::OriginalApplicationEntities50LA> app,
    rr::platform::RaceAudioGroups50IC& audio,rr::platform::AudioMixer50HD& mixer,
    std::uint64_t generation,std::uint32_t& pc)noexcept {
    pc=0;
    if(worldColdAttempted_||topologyAttempted_||topologyClosed_||!app||!app->application10()||
       !generation||app->attachmentLock423F58()||!audio.coldStarted_||audio.coldRetiring_||
       audio.coldNext_!=6||audio.coldApplication_!=app||audio.coldSession_!=generation||
       audio.coldMixer_!=&mixer||audio.coldThread_!=std::this_thread::get_id()||
       audio.coldEpoch_!=mixer.groupEpoch50IC()||audio.mixer_||audio.groups_.initialized)
        return fail(pc,0x1a6bd4);
    for(std::size_t i=0;i<mixer.count();++i)
        if(mixer.active(i)||mixer.hasVoiceGroupGain50IC(i))return fail(pc,0x26fcf8);
    const auto worldKey=key(&worldEntity_);
    if(!app->entityAuthority_.canEnroll(worldEntity_,worldKey)||
       app->objectCount423F60()==std::numeric_limits<std::uint32_t>::max())return fail(pc,0x141300);
    std::unique_ptr<Entity*[]> list;
    try {list.reset(new Entity*[10]);}catch(...){return fail(pc,0x1b5f64);}
    worldColdAttempted_=true;worldApplication_=std::move(app);worldAudio_=&audio;
    const auto r=g::initializeOriginalWreckComponent50LA(worldEntity_,
        g::OriginalWreckComponentKind50LA::entity141300,generation,worldKey,true,
        worldApplication_->objectCount423F60());
    if(r.status!=g::OriginalWreckComponentStatus50LA::complete)std::terminate();
    if(!worldApplication_->entityAuthority_.enroll(worldEntity_))std::terminate();
    worldColdConstructed_=true;worldMode30_=1; //26FCBC before actual270550 reset.
    // The same scene has not reached any loader/topology. Its native owners
    // are genuinely absent; do not invent values for uninitialized3CC/3D4.
    useEnvironment35C_=0;
    if(!audio.constructNextCold2350D0(mixer,worldApplication_,generation,g::RaceAudioGroup50IC::World4D8))
        return fail(pc,0x26fcf8); //Retain world prefix; explicit native rollback below.
    worldList500_=std::move(list);worldListCapacity504_=10;worldListGrowth508_=100000;
    worldListCount50C_=0;
    worldApplication_->worldRegistration434320_=0; //1B28D0, real shared word.
    worldMode524_=3; //1B28D4 follows the process-word clear.
    pc=0x1b2918;return true; //Constructor prefix only, never world-load readiness.
}
inline bool Owner::rollbackNativeWorldConstructionFailure(std::uint32_t& pc)noexcept {
    pc=0;
    if(!worldColdConstructed_||!worldApplication_||!worldAudio_||topologyAttempted_||
       topologyConstructed_||trackParticles_||environmentMethod_||worldListCount50C_||
       worldApplication_->attachmentLock423F58())return fail(pc,0x1b2870);
    auto& e=worldEntity_;auto& audio=*worldAudio_;
    if(e.reference00!=1||e.previous10||e.next14||e.parent1C||e.firstChild18||e.objectKey!=key(&e)||
       audio.coldApplication_!=worldApplication_||audio.coldSession_!=e.sessionGeneration||
       audio.coldThread_!=std::this_thread::get_id()||!worldApplication_->objectCount423F60())
        return fail(pc,0x1b2870);
    auto& group=audio.coldGroups_[static_cast<std::size_t>(g::RaceAudioGroup50IC::World4D8)];
    if(group.constructed&&!audio.releaseEmptyCold142590(g::RaceAudioGroup50IC::World4D8))
        return fail(pc,0x235150);
    // Host failure cleanup only. The complete world destructor is NOT called:
    //3CC/3D4, LightManager148110 and context103DD0 have no false completion.
    if(!worldApplication_->entityAuthority_.retire(e))std::terminate();
    worldList500_.reset();--e.reference00;--worldApplication_->objectCount423F60();
    worldColdConstructed_=false;worldAudio_=nullptr;worldApplication_.reset();
    return true;
}
}
namespace ra_cold_aggregate50LA {
inline bool Owner::constructWorldCold1B2870(const original_main_entry50KG::Application& app,
    rr::platform::RaceAudioGroups50IC& audio,rr::platform::AudioMixer50HD& mixer,
    std::uint64_t generation,std::uint32_t& pc)noexcept {
    pc=0;
    // Authenticated source prewarming may precede counted construction, but
    // no existing counted model/particle/scene prefix may be reordered here.
    if(closed_||terminal_||!source_||!prefix_.sources||trackParticleAttempted_||
       modelPrefixesAttempted_||particlesAttempted_||runtimeCameraBorrows_||runtimeWorldBorrows_){
        pc=0x1a6bd4;return false;
    }
    return constructWorldColdPrefix(app.entities(),audio,mixer,generation,pc);
}
}

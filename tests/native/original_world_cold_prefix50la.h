#pragma once
#include "original_ui_main_entry50kg.h"
#include "ra_cold_aggregate50la.h"
#include <limits>
namespace ra_pod_scene50LA {
inline bool Owner::prepareWorldPhysicsIndex270CDC(
    std::shared_ptr<const ra_cold_sources50LA::Owner> source,std::uint32_t& pc)noexcept {
    pc=0;
    if(!worldColdConstructed_||!worldList500_||worldPhysics3D4_||topologyAttempted_)
        return fail(pc,0x270cc4);
    std::unique_ptr<g::OriginalWorldPhysicsIndex50LA> index;
    try {index.reset(new g::OriginalWorldPhysicsIndex50LA);}catch(...){return fail(pc,0x270cc4);}
    if(!index->prepare27C110(std::move(source)))return fail(pc,0x270ce4);
    worldPhysics3D4_=std::move(index);return true;
}

inline bool Owner::prepareWorldCollisionObjects271230(
    std::shared_ptr<const ra_cold_sources50LA::Owner> source,std::uint32_t& pc)noexcept {
    pc=0;
    if(!worldColdConstructed_||!worldList500_||!worldPhysics3D4_||worldCollisionObjects3CC_||
       topologyAttempted_||!worldApplication_||worldApplication_->attachmentLock423F58()||
       !worldAudio_||worldAudio_->coldThread_!=std::this_thread::get_id())return fail(pc,0x271230);
    auto& authority=worldApplication_->entityAuthority_;
    if(worldApplication_->objectCount423F60()>std::numeric_limits<std::uint32_t>::max()-10||
       !authority.reserveAdditional(10))
        return fail(pc,0x141300);
    std::unique_ptr<g::OriginalWorldCollisionObjects50LA> objects;
    try {objects.reset(new g::OriginalWorldCollisionObjects50LA);}catch(...){return fail(pc,0x271264);}
    if(!objects->prepareBacking(std::move(source)))return fail(pc,0x27bca0);
    for(auto& zone:objects->zones_)
        if(!authority.canEnroll(zone.entity,key(&zone.entity)))return fail(pc,0x141300);
    // Native backing/admission above has no counted effects. Actual zone
    // constructors and their count30 publications retain source order below.
    worldCollisionObjects3CC_=std::move(objects);
    for(auto& zone:worldCollisionObjects3CC_->zones_){
        auto& e=zone.entity;
        if(g::initializeOriginalWreckComponent50LA(e,g::OriginalWreckComponentKind50LA::collisionObjects27BB40,
            worldEntity_.sessionGeneration,key(&e),true,worldApplication_->objectCount423F60()).status!=
            g::OriginalWreckComponentStatus50LA::complete||!authority.enroll(e))std::terminate();
        zone.counted=true;zone.count30=static_cast<std::uint32_t>(zone.entries34.size());
    }
    return true;
}
inline bool Owner::canReleaseWorldCollisionObjects27BB80(std::uint32_t& pc)const noexcept {
    if(!worldCollisionObjects3CC_)return true;
    if(!worldApplication_||worldApplication_->attachmentLock423F58())return fail(pc,0x27bb80);
    auto& objects=*worldCollisionObjects3CC_;
    std::size_t counted=0;
    for(const auto& zone:objects.zones_){const auto& e=zone.entity;
        if(!zone.counted||e.reference00!=1||e.previous10||e.next14||e.parent1C||e.firstChild18||
           e.mask20||e.descendants24||e.objectKey!=key(&e)||e.sessionGeneration!=worldEntity_.sessionGeneration||
           zone.count30!=zone.entries34.size()||!worldApplication_->entityAuthority_.canRetire(e))
            return fail(pc,0x27bb80);
        for(const auto& wrapper:zone.entries34)if(wrapper.collision04||!wrapper.descriptor08)
            return fail(pc,0x27bbf4);
        ++counted;
    }
    if(worldApplication_->objectCount423F60()<counted)return fail(pc,0x1413e0);
    return true;
}
inline bool Owner::releaseWorldCollisionObjects27BB80(std::uint32_t& pc)noexcept {
    if(!canReleaseWorldCollisionObjects27BB80(pc))return false;
    if(!worldCollisionObjects3CC_)return true;
    auto& objects=*worldCollisionObjects3CC_;
    for(auto& zone:objects.zones_){
        //Actual142590/104970 final reference, then27BB80 raw descriptor/row
        //deletion and1413E0. No live collision Entity has been acquired here.
        if(!worldApplication_->entityAuthority_.retire(zone.entity))std::terminate();
        --zone.entity.reference00;
        for(auto& wrapper:zone.entries34){wrapper.descriptor08.reset();wrapper.decodedCollision=nullptr;}
        decltype(zone.entries34)().swap(zone.entries34);
        --worldApplication_->objectCount423F60();zone.counted=false;
    }
    worldCollisionObjects3CC_.reset();return true;
}

inline bool Owner::prepareWorldCollisionResources(std::shared_ptr<ra_static_candidates50LA::Owner> candidates,
    std::shared_ptr<rr::assets::OriginalStandardTextureResolver50LA> resolver,
    rr::assets::OriginalStandardTextureDiagnostic50LA& d)noexcept {
    if(!worldColdConstructed_||!worldCollisionObjects3CC_||worldCollisionResources_||topologyAttempted_||
       !worldAudio_||worldAudio_->coldThread_!=std::this_thread::get_id())return false;
    // Join the actual3CC wrappers to the SAME retained decoded source. Matching
    // filenames/counts from another aggregate cannot authorize these shapes.
    const auto* source=worldCollisionObjects3CC_->source_->sourcesDecoded();
    if(!candidates||!source||candidates->decodedSource()!=source)return false;
    std::size_t collision=0;
    for(const auto& zone:worldCollisionObjects3CC_->zones_){
        if(!zone.counted||zone.count30!=zone.entries34.size())return false;
        for(const auto& wrapper:zone.entries34){
            if(wrapper.type00!=1||wrapper.collision04||!wrapper.descriptor08)return false;
            if(!wrapper.decodedCollision)continue; //Still a future actual resolver miss.
            if(collision>=source->collisions.size()||
               wrapper.decodedCollision!=&source->collisions[collision])return false;
            const auto& authored=*wrapper.descriptor08->source;
            if(wrapper.decodedCollision->zone!=authored.zone||
               wrapper.decodedCollision->ordinal!=authored.ordinal)return false;
            ++collision;
        }
    }
    if(collision!=source->collisions.size())return false;
    try {worldCollisionResources_.reset(new ra_collision_resources50LA::Owner);}catch(...){return false;}
    return worldCollisionResources_->prepare(std::move(candidates),std::move(resolver),d);
}
inline bool Owner::constructWorldColdPrefix(std::shared_ptr<g::OriginalApplicationEntities50LA> app,
    rr::platform::RaceAudioGroups50IC& audio,rr::platform::AudioMixer50HD& mixer,
    std::uint64_t generation,std::uint32_t& pc)noexcept {
    pc=0;
    if(worldColdAttempted_||topologyAttempted_||topologyClosed_||!app||!app->application10()||
       !generation||(collisionApplication_&&collisionApplication_!=app)||
       app->attachmentLock423F58()||!audio.coldStarted_||audio.coldRetiring_||
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
// Explicit native ownership retirement, not a26FD80 dispatch or substitute
// for unrepresented LightManager/context callbacks. The aggregate calls the
// second leaf only after its actual runtime/GPU/static owners have closed.
inline bool Owner::canRetireNativeWorldPrefix(const char** gate)const noexcept {
    const auto reject=[&](const char* text){if(gate)*gate=text;return false;};
    if(!worldColdConstructed_||!worldApplication_||!worldAudio_||
       worldApplication_->attachmentLock423F58())return reject("world prefix actual owner/lock");
    const auto& e=worldEntity_;const auto& audio=*worldAudio_;
    if(e.reference00!=1||e.previous10||e.next14||e.parent1C||e.firstChild18||e.mask20||e.descendants24||
       e.objectKey!=key(&e)||audio.coldApplication_!=worldApplication_||
       audio.coldSession_!=e.sessionGeneration||audio.coldThread_!=std::this_thread::get_id()||
       !audio.coldMixer_||audio.coldEpoch_!=audio.coldMixer_->groupEpoch50IC())return reject("world prefix Entity/audio identity");
    if(!worldApplication_->entityAuthority_.canRetire(e))return reject("world prefix Entity authority");
    if(worldList500_&&worldListCount50C_)return reject("world prefix nonempty list500");
    if(worldCollisionResources_&&!worldCollisionResources_->canCloseUnloadedBacking())return reject("world collision backing still loaded");
    std::uint32_t pc=0;if(!canReleaseWorldCollisionObjects27BB80(pc))return reject("world collision object topology");
    if(worldPhysics3D4_&&!worldPhysics3D4_->canRelease27C030())return reject("world physics index references");
    const auto& group=audio.coldGroups_[static_cast<std::size_t>(g::RaceAudioGroup50IC::World4D8)];
    if(group.constructed&&!audio.canReleaseEmptyCold142590(g::RaceAudioGroup50IC::World4D8))return reject("world seventh audio group retirement");
    const std::size_t required=1u+std::size_t(group.constructed)+
        (worldCollisionObjects3CC_?worldCollisionObjects3CC_->zones_.size():0u);
    if(worldApplication_->objectCount423F60()<required)return reject("world prefix Application count");
    return true;
}
inline bool Owner::retireNativeWorldPrefixAfterOwnedCleanup(std::uint32_t& pc)noexcept {
    pc=0;
    if(!canRetireNativeWorldPrefix()||runtimeQueries_||trackParticles_||environmentMethod_||
       (topologyAttempted_&&!topologyClosed_))return fail(pc,0x1b2870);
    if(worldCollisionResources_&&!worldCollisionResources_->closeUnloadedBacking())return fail(pc,0x11b600);
    worldCollisionResources_.reset();
    if(!releaseWorldCollisionObjects27BB80(pc))return false;
    auto& audio=*worldAudio_;
    const auto group=g::RaceAudioGroup50IC::World4D8;
    if(audio.coldGroups_[static_cast<std::size_t>(group)].constructed&&
       !audio.releaseEmptyCold142590(group))return fail(pc,0x235150);
    if(worldPhysics3D4_&&!worldPhysics3D4_->release27C030())return fail(pc,0x27c030);
    worldPhysics3D4_.reset();
    if(!worldApplication_->entityAuthority_.retire(worldEntity_))return fail(pc,0x1413e0);
    worldList500_.reset();--worldEntity_.reference00;--worldApplication_->objectCount423F60();
    worldColdConstructed_=false;worldAudio_=nullptr;worldApplication_.reset();return true;
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
    if(worldCollisionResources_&&!worldCollisionResources_->closeUnloadedBacking())return fail(pc,0x11b600);
    worldCollisionResources_.reset();
    if(!releaseWorldCollisionObjects27BB80(pc))return false;
    auto& group=audio.coldGroups_[static_cast<std::size_t>(g::RaceAudioGroup50IC::World4D8)];
    if(group.constructed&&!audio.releaseEmptyCold142590(g::RaceAudioGroup50IC::World4D8))
        return fail(pc,0x235150);
    // Host failure cleanup only. The complete world destructor is NOT called:
    //Later live3CC loading, LightManager148110 and context103DD0 remain unjoined.
    //Only an actually acquired, source-proven3D4 index is retired here.
    if(worldPhysics3D4_&&!worldPhysics3D4_->release27C030())return fail(pc,0x27c030);
    worldPhysics3D4_.reset();
    if(!worldApplication_->entityAuthority_.retire(e))std::terminate();
    worldList500_.reset();--e.reference00;--worldApplication_->objectCount423F60();
    worldColdConstructed_=false;worldAudio_=nullptr;worldApplication_.reset();
    return true;
}
}
namespace ra_cold_aggregate50LA {
inline bool Owner::prepareWorldCollisionResources(const original_main_entry50KG::Application& app,
    rr::assets::OriginalStandardTextureDiagnostic50LA& d)noexcept {
    if(closed_||terminal_||!candidates_||!worldColdConstructed_||app.entities()!=worldApplication_||
       runtimeCameraBorrows_||runtimeWorldBorrows_)return false;
    try {
        const auto domain=app.standardMaterialDomain();
        if(!domain||!domain->prepared())return false;
        return ra_pod_scene50LA::Owner::prepareWorldCollisionResources(candidates_,domain->resolver3E1A70(),d);
    }catch(...){return false;}
}
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

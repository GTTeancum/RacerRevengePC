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
    if(!canRetireNativeCollisionQueries27B040())return fail(pc,0x27bbf4);
    std::size_t counted=0;
    for(const auto& zone:objects.zones_){const auto& e=zone.entity;
        if(!zone.counted||e.reference00!=1||e.previous10||e.next14||e.parent1C||e.firstChild18||
           e.mask20||e.descendants24||e.objectKey!=key(&e)||e.sessionGeneration!=worldEntity_.sessionGeneration||
           zone.count30!=zone.entries34.size()||!worldApplication_->entityAuthority_.canRetire(e))
            return fail(pc,0x27bb80);
        for(const auto& wrapper:zone.entries34){
            if(wrapper.nativeQuery){
                if(wrapper.descriptorRetired!=!bool(wrapper.descriptor08))return fail(pc,0x27bbf4);
            }else if(wrapper.collision04||!wrapper.descriptor08||wrapper.descriptorRetired)
                return fail(pc,0x27bbf4);
        }
        ++counted;
    }
    if(worldApplication_->objectCount423F60()<counted)return fail(pc,0x1413e0);
    return true;
}
inline bool Owner::releaseWorldCollisionObjects27BB80(std::uint32_t& pc)noexcept {
    if(!canReleaseWorldCollisionObjects27BB80(pc))return false;
    if(!worldCollisionObjects3CC_)return true;
    auto& objects=*worldCollisionObjects3CC_;
    for(const auto& zone:objects.zones_)for(const auto& w:zone.entries34)
        if(w.collision04||(w.nativeQuery&&!w.nativeQuery->retired))return fail(pc,0x27bbf4);
    for(auto& zone:objects.zones_){
        //Actual142590/104970 final reference, then27BB80 raw descriptor/row
        //deletion and1413E0. Any reached collision Entity retired separately first.
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
    std::uint64_t generation,const track_height_cold50JB::Owner& height,std::uint32_t& pc)noexcept {
    pc=0;
    if(worldColdAttempted_||nativeWaterDomain_||topologyAttempted_||topologyClosed_||!app||!app->application10()||
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
    worldTrackEmitsDust51C_=0u; //1B28DC, after524 and before the returned prefix.
    //Explicit native domain allocation after actual counted constructor
    //effects. Not guest270550/reset, no native water resource is allocated.
    try {nativeWaterDomain_=std::shared_ptr<native_world_water50LA::Domain>(
        new native_world_water50LA::Domain(*worldApplication_,worldEntity_,generation,height));}
    catch(...){return fail(pc,0x1b2918);} //Existing counted rollback owns reached effects.
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
    if(nativeWaterDomain_&&!nativeWaterDomain_->canRetire(*worldApplication_,e,e.sessionGeneration))
        return reject("native water constructor domain identity");
    if(worldList500_&&worldListCount50C_)return reject("world prefix nonempty list500");
    if(!canRetireNativeCollisionQueries27B040())return reject("world collision query/resource custody");
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
    if(nativeWaterDomain_){nativeWaterDomain_->revoke();nativeWaterDomain_.reset();}
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
    if(nativeWaterDomain_){
        if(!nativeWaterDomain_->canRetire(*worldApplication_,e,e.sessionGeneration))return fail(pc,0x1b2870);
        nativeWaterDomain_->revoke();nativeWaterDomain_.reset();
    }
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
    if(!source_->sourcesDecoded()){pc=0x1a6bd4;return false;}
    return constructWorldColdPrefix(app.entities(),audio,mixer,generation,
        source_->sourcesDecoded()->height,pc);
}
}

namespace ra_pod_scene50LA {
inline bool Owner::loadNativeEmptyAnimations2713E0(
    const rr::assets::ParameterDocument48F& document,std::uint32_t& pc)noexcept {
    pc=0x271d34;
    if(!worldColdConstructed_||!worldApplication_||!worldAudio_||
       worldAudio_->coldThread_!=std::this_thread::get_id()||
       worldApplication_->attachmentLock423F58()||nativeEmptyToyList3C8_)return false;
    //Native bounded-path admission from the WHOLE authenticated RA document,
    //not the source157D00 call yet. A present section needs real toy ownership
    //and is deliberately unsupported. Do not create then abandon a counted list.
    if(document.blocks.size()!=713u)return false;
    for(const auto& block:document.blocks)if(block.name=="animations")return false;
    auto& authority=worldApplication_->entityAuthority_;
    auto& count=worldApplication_->objectCount423F60();
    pc=0x2713f8;
    if(count==std::numeric_limits<std::uint32_t>::max()||!authority.reserveAdditional(1))return false;
    std::unique_ptr<NativeEmptyToyList3C8> list;
    try {list=std::make_unique<NativeEmptyToyList3C8>();}catch(...){return false;}
    auto& entity=list->entity;
    if(!authority.canEnroll(entity,key(&entity)))return false;
    pc=0x2d9b4c; //2D9B40(a1=1)->141300: SAME actual Application count/authority.
    if(g::initializeOriginalWreckComponent50LA(entity,g::OriginalWreckComponentKind50LA::toyList2D9B40,
       worldEntity_.sessionGeneration,key(&entity),true,count).status!=
       g::OriginalWreckComponentStatus50LA::complete||!authority.enroll(entity))std::terminate();
    list->count30=0; //2D9B60, source34 null; no toy storage allocated.
    nativeEmptyToyList3C8_=std::move(list); //271414 publication AFTER constructor.
    pc=0x2d9c94; //271424/+74->2D9C60->157D00("Animations").
    //Immutable whole-document absence was admitted above. Source2D9CA4 logs
    //missing section and returns0 at2D9CB4; no NumberOfAnimations/default read or
    //toy allocation is reached. This native leaf does not reproduce debug IO.
    pc=0x271438;
    if(nativeEmptyToyList3C8_->count30||entity.reference00!=1||entity.parent1C||
       entity.previous10||entity.next14||entity.firstChild18||entity.mask20||entity.descendants24||
       !count||!authority.canRetire(entity))std::terminate();
    pc=0x27144c; //slot10->142590->104970, final release->2D9B80->1413E0.
    //No toy array34 means2D9BB4 skips its destructor loop. This exact empty
    //leaf needs no generic animation callbacks or child destruction guesses.
    if(!authority.retire(entity))std::terminate();
    --entity.reference00;--count;
    nativeEmptyToyList3C8_.reset(); //271454 World3C8 clear after destruction.
    pc=0x271d44;return true; //STOP before third A4 call/+A8.
}
}

namespace ra_pod_scene50LA {
inline bool Owner::createNativeTriggerList2DC320(
    std::shared_ptr<ra_static_candidates50LA::Owner> candidates,
    std::shared_ptr<const ra_trigger_authoring50LA::Owner> authoring,std::uint32_t& pc)noexcept {
    pc=0x1b5010;
    if(nativeTriggerList3D0_)return nativeTriggerList3D0_->candidates==candidates&&
        nativeTriggerList3D0_->authoring==authoring;
    if(!worldColdConstructed_||!worldApplication_||!worldAudio_||
       worldAudio_->coldThread_!=std::this_thread::get_id()||worldApplication_->attachmentLock423F58()||
       !candidates||!authoring||candidates->triggerSource_!=authoring||!candidates->triggerActions_||
       candidates->triggerActions_->source_!=authoring.get())return false;
    auto& actions=*candidates->triggerActions_;
    if(!actions.ready_||actions.lease_||actions.terminal_!=ra_trigger_actions50LA::Status::Complete)return false;
    for(const auto& pair:actions.nativeEntities_)if(pair.triggerCounted||pair.colliderCounted)return false;
    auto& authority=worldApplication_->entityAuthority_;auto& count=worldApplication_->objectCount423F60();
    if(count>std::numeric_limits<std::uint32_t>::max()-5u||!authority.reserveAdditional(5))return false;
    std::unique_ptr<NativeTriggerList3D0> list;
    try {list=std::make_unique<NativeTriggerList3D0>();}catch(...){return false;}
    list->candidates=std::move(candidates);list->authoring=std::move(authoring);
    list->actions=list->candidates->triggerActions_;
    auto& entity=list->entity;
    if(!authority.canEnroll(entity,key(&entity)))return false;
    pc=0x2dc32c;
    if(g::initializeOriginalWreckComponent50LA(entity,g::OriginalWreckComponentKind50LA::triggerList2DC320,
       worldEntity_.sessionGeneration,key(&entity),true,count).status!=
       g::OriginalWreckComponentStatus50LA::complete||!authority.enroll(entity))std::terminate();
    list->counted=true; //count30 defaults0 and descriptors34 null at2DC340/344.
    nativeTriggerList3D0_=std::move(list); //1B502C BEFORE2DC430 reader.
    return true;
}

inline bool Owner::countNativeTriggerPair2A0290(std::size_t i,std::uint32_t& pc)noexcept {
    pc=0x2dbff4;
    if(!nativeTriggerList3D0_||i>=2u||!worldApplication_||worldApplication_->attachmentLock423F58())return false;
    auto& list=*nativeTriggerList3D0_;auto& actions=*list.actions;auto& pair=actions.nativeEntities_[i];
    auto& candidate=list.candidates->candidates_[178u+i];auto& state=actions.group_.triggers[i];
    auto& authority=worldApplication_->entityAuthority_;auto& count=worldApplication_->objectCount423F60();
    if(actions.lease_||!actions.ready_||candidate.nativeTriggers!=&actions.group_||
       candidate.nativeTriggerIndex!=i)return false;
    if(!pair.triggerCounted){
        if(count==std::numeric_limits<std::uint32_t>::max()||!authority.reserveAdditional(2)||
           !authority.canEnroll(pair.trigger,key(&pair.trigger)))return false;
        //Only missing Entity lifetime is initialized. Existing trigger flags,
        //contacts, geometry, actions and membership are NEVER reinitialized.
        if(g::initializeOriginalWreckComponent50LA(pair.trigger,g::OriginalWreckComponentKind50LA::trigger2A0290,
           worldEntity_.sessionGeneration,key(&pair.trigger),true,count).status!=
           g::OriginalWreckComponentStatus50LA::complete||!authority.enroll(pair.trigger))std::terminate();
        pair.triggerCounted=true;pair.trigger.nativeFlags29=&state.flags29;
    }
    pc=0x2a0534;
    if(!pair.colliderCounted){
        if(count==std::numeric_limits<std::uint32_t>::max()||!authority.reserveAdditional(1)||
           !authority.canEnroll(pair.collider,key(&pair.collider)))return false;
        //11B208 counts the embedded collider Entity+8, not another shape.
        if(g::initializeOriginalWreckComponent50LA(pair.collider,g::OriginalWreckComponentKind50LA::collision11B190,
           worldEntity_.sessionGeneration,key(&pair.collider),true,count).status!=
           g::OriginalWreckComponentStatus50LA::complete||!authority.enroll(pair.collider))std::terminate();
        pair.colliderCounted=true;pair.collider.nativeFlags29=&candidate.candidate.collision.filter.flags31;
    }
    pc=0x2a073c;
    if(!pair.attached){
        Entity* nodes[]={&pair.trigger,&pair.collider};
        const g::OriginalWreckComponentDomain50LA domain{worldEntity_.sessionGeneration,nodes,2};
        if(g::appendOriginalWreckComponent50LA(domain,pair.trigger,pair.collider,2u,
           worldApplication_->attachmentLock423F58()).status!=g::OriginalWreckComponentStatus50LA::complete)return false;
        pair.attached=true; //No reference increment; no list/world parenting.
    }
    return true;
}

inline bool Owner::canRetireNativeTriggerList3D0()const noexcept {
    if(!nativeTriggerList3D0_)return true;
    if(!worldApplication_||worldApplication_->attachmentLock423F58())return false;
    const auto& list=*nativeTriggerList3D0_;
    if(!list.counted||!list.candidates||!list.authoring||!list.actions||list.joined>2u||list.count30>2u||
       list.actions!=list.candidates->triggerActions_||list.authoring!=list.candidates->triggerSource_||
       list.actions->lease_||list.servicesCompleted>2u||(list.serviceComplete&&list.servicesCompleted!=2u)||
       !list.actions->canRetireNativeLostAiCallbacks()||!canRetireNativeTriggerObservers())return false;
    const auto& authority=worldApplication_->entityAuthority_;
    const auto base=[&](const Entity& e,g::OriginalWreckComponentKind50LA kind){
        return e.kind==kind&&e.reference00==1u&&e.objectKey==key(&e)&&
            e.sessionGeneration==worldEntity_.sessionGeneration&&e.category28==0xffu&&authority.canRetire(e);
    };
    const auto& root=list.entity;
    if(!base(root,g::OriginalWreckComponentKind50LA::triggerList2DC320)||root.parent1C||
       root.previous10||root.next14||root.mask20||root.nativeFlags29)return false;
    std::size_t counted=1;const Entity* first=nullptr;const Entity* previous=nullptr;
    for(std::size_t i=0;i<2u;++i){const auto& pair=list.actions->nativeEntities_[i];
        const auto& t=pair.trigger;const auto& c=pair.collider;
        if(pair.colliderCounted&&!pair.triggerCounted)return false;
        if(pair.attached&&!pair.colliderCounted)return false;
        if(list.parented[i]&&(!pair.attached||!list.geometryRoleConsumed[i]))return false;
        if(i<list.servicesCompleted&&!list.parented[i])return false;
        if(pair.triggerCounted){
            if(!base(t,g::OriginalWreckComponentKind50LA::trigger2A0290)||
               t.nativeFlags29!=&list.actions->group_.triggers[i].flags29||
               t.firstChild18!=(pair.attached?&c:nullptr)||t.descendants24!=(pair.attached?2u:0u))return false;
            if(list.parented[i]){
                if(t.parent1C!=&root||t.previous10!=previous||t.mask20!=0x402u||
                   (previous&&previous->next14!=&t))return false;
                if(!first)first=&t;previous=&t;
            }else if(t.parent1C||t.previous10||t.next14||t.mask20)return false;
            ++counted;
        }
        if(pair.colliderCounted){
            if(!base(c,g::OriginalWreckComponentKind50LA::collision11B190)||c.firstChild18||c.descendants24||
               c.previous10||c.next14||
               c.nativeFlags29!=&list.candidates->candidates_[178u+i].candidate.collision.filter.flags31||
               c.parent1C!=(pair.attached?&t:nullptr)||c.mask20!=(pair.attached?2u:0u))return false;
            ++counted;
        }
    }
    return root.firstChild18==first&&(!previous||!previous->next14)&&
        root.descendants24==(first?0x402u:0u)&&worldApplication_->objectCount423F60()>=counted;
}

inline bool Owner::resetNativeTriggerList2DCC70(
    const std::shared_ptr<g::OriginalApplicationEntities50LA>& expected,
    std::uint64_t generation,std::uint32_t& pc)noexcept {
    pc=0x2dcc70;
    // Immutable actual Application affinity BEFORE this World's mutable
    // slots or flags. An equal generation/descriptor name is not authority.
    if(!expected||expected->nativeOwnerThread_!=std::this_thread::get_id())return false;
    if(worldApplication_!=expected||!generation||!worldColdConstructed_||topologyClosed_||
       worldEntity_.sessionGeneration!=generation||worldEntity_.reference00!=1u||
       !expected->entityAuthority_.canRetire(worldEntity_)||!nativeTriggerList3D0_||
       !canRetireNativeTriggerList3D0())return false;
    auto& list=*nativeTriggerList3D0_;
    // The actual completed RA TOB reader owns exactly these two descriptors,
    // their sole live action state and counted Trigger/collider identities.
    // Reuse custody checks without performing ANY retirement/re-attachment.
    if(!list.complete||list.count30!=2u||list.joined!=2u||
       !list.authoring->sources()||list.actions->source_!=list.authoring.get()||
       !list.actions->ready_||list.actions->lease_||
       list.actions->terminal_!=ra_trigger_actions50LA::Status::Complete||
       list.actions->group_.triggers.size()!=list.count30)return false;
    for(std::size_t i=0;i<list.count30;++i){
        const auto& pair=list.actions->nativeEntities_[i];
        if(list.descriptors34[i]!=&(*list.authoring->sources())[i]||
           !pair.triggerCounted||!pair.colliderCounted||!pair.attached||
           pair.trigger.nativeFlags29!=&list.actions->group_.triggers[i].flags29)return false;
    }
    //2DCC70 computes zero for bit3, traverses descriptor34->Trigger00,
    // reads flags1B8, preserves all other bits and stores at2DCCAC.
    // No flags29, contacts, ignored colliders, callback state, C80, geometry,
    // query membership, audio/script body or diagnostic counters are reset.
    for(std::size_t i=0;i<list.count30;++i){
        auto& flags=list.actions->group_.triggers[i].flags1B8;
        flags=static_cast<std::uint8_t>(flags&0xf7u);
    }
    pc=0x2dccc0;return true; // This leaf returned, NOT World/Race restart.
}

inline bool Owner::retireNativeTriggerList3D0()noexcept {
    if(!nativeTriggerList3D0_)return true;
    // Actual Storage alone retires registration/membership. Source-owner
    // raw borrows were withdrawn before topology/storage destruction.
    if(runtimeQueries_||!canRetireNativeTriggerList3D0())return false;
    auto& list=*nativeTriggerList3D0_;auto& authority=worldApplication_->entityAuthority_;
    auto& count=worldApplication_->objectCount423F60();
    for(std::size_t n=2u;n>0u;--n){const auto i=n-1u;auto& pair=list.actions->nativeEntities_[i];
        std::uint32_t pc=0;
        //142590 child-first: collider11B390 owns its actual E0 observer, not
        //a second native registry. Pool cleanup cannot dereference dead Storage.
        if(!retireNativeTriggerObserver2A0420(i,pc))std::terminate();
        if(pair.colliderCounted){
            if(pair.attached)pair.trigger.firstChild18=nullptr;
            if(!authority.retire(pair.collider))std::terminate();
            pair.collider.nativeFlags29=nullptr;--pair.collider.reference00;--count;pair.colliderCounted=false;
            pair.collider.parent1C=nullptr;pair.attached=false; //Dead native borrow withdrawal.
        }
        if(pair.triggerCounted){
            //Actual last-child1424E0/142590 link stores; no141A70 traversal
            //or dying-parent mask recomputation between these final releases.
            if(list.parented[i]){
                if(pair.trigger.previous10)pair.trigger.previous10->next14=nullptr;
                else list.entity.firstChild18=nullptr;
            }
            if(!list.actions->retireNativeLostAiCallback2A0420(i,pc))std::terminate();
            if(!authority.retire(pair.trigger))std::terminate();
            pair.trigger.nativeFlags29=nullptr;--pair.trigger.reference00;--count;pair.triggerCounted=false;
            pair.trigger.parent1C=pair.trigger.previous10=nullptr;list.parented[i]=false;
        }
        list.descriptors34[i]=nullptr;
    }
    //Descriptor destruction does not own another Trigger reference. Existing
    //immutable authoring remains alive; consumed native geometry role is not a
    //claim that the omitted original50-byte allocation was freed by this host.
    if(!authority.retire(list.entity))std::terminate();
    --list.entity.reference00;--count;list.counted=false;
    nativeTriggerList3D0_.reset();return true;
}

}

namespace ra_pod_scene50LA {
inline bool Owner::beginNativeSunFlareDomain271E5C(std::uint32_t& pc)noexcept {
    pc=0x271e5c;
    if(!worldColdConstructed_||!worldApplication_||!worldAudio_||
       worldAudio_->coldThread_!=std::this_thread::get_id()||
       worldApplication_->attachmentLock423F58()||!worldEntity_.sessionGeneration)return false;
    if(!nativeSunFlares_){
        //Explicit native join: this newly implemented private domain has never
        //owned a flare. No source270550 reset, existing-state clear or adoption
        //of unrelated sunlight/fire/garage owners is claimed or performed.
        nativeSunFlares_.emplace();nativeSunFlares_->generation=worldEntity_.sessionGeneration;
    }
    auto& d=*nativeSunFlares_;
    if(d.generation!=worldEntity_.sessionGeneration)return false;
    if(d.count4A8&&*d.count4A8!=1u)return false;
    pc=0x271e68;d.count4A8=1u; //Source store before allocation; retained on failure.
    pc=0x271e74;
    if(!d.pointers4A4){
        if(d.owner)return false;
        try {d.pointers4A4=std::make_unique<NativeSunFlare*[]>(1);}catch(...){return false;}
        //Native null entry marks unconstructed ownership; source array element
        //is not claimed initialized until27209C publishes its real object.
    }
    return true;
}
inline bool Owner::constructNativeSunFlare2C5730(NativeSunFlareInputs inputs,std::uint32_t& pc)noexcept {
    pc=0x27206c;
    if(!nativeSunFlares_||!worldApplication_||worldApplication_->attachmentLock423F58())return false;
    auto& d=*nativeSunFlares_;
    if(d.generation!=worldEntity_.sessionGeneration||!d.count4A8||*d.count4A8!=1||!d.pointers4A4)return false;
    if(d.owner){
        const auto& old=d.owner->inputs;
        return canRetireNativeSunFlare()&&old.texture==inputs.texture&&old.retrieveMaterial==inputs.retrieveMaterial&&
            old.modeA1==inputs.modeA1&&old.saturation==inputs.saturation&&old.minScale==inputs.minScale&&
            old.maxScale==inputs.maxScale&&old.width==inputs.width&&old.height==inputs.height&&
            old.vertexColor==inputs.vertexColor&&old.direction==inputs.direction;
    }
    if(d.pointers4A4[0])return false;
    auto& authority=worldApplication_->entityAuthority_;auto& count=worldApplication_->objectCount423F60();
    if(count==std::numeric_limits<std::uint32_t>::max()||!authority.reserveAdditional(1))return false;
    std::unique_ptr<NativeSunFlare> object;
    try {object=std::make_unique<NativeSunFlare>();object->inputs=std::move(inputs);}catch(...){return false;}
    auto& e=object->entity;
    if(!authority.canEnroll(e,key(&e)))return false;
    pc=0x2c5740;
    if(g::initializeOriginalWreckComponent50LA(e,g::OriginalWreckComponentKind50LA::entity141300,
       worldEntity_.sessionGeneration,key(&e),true,count).status!=g::OriginalWreckComponentStatus50LA::complete||
       !authority.enroll(e))std::terminate();
    object->counted=true;
    //2C5730 constructor writes represented above;245CF0 is an uncounted,
    //empty embedded material handle, not a new Entity or live material lease.
    d.owner=std::move(object);
    pc=0x27209c;d.pointers4A4[0]=d.owner.get(); //Publication BEFORE2C5A10.
    return true;
}
inline bool Owner::canRetireNativeSunFlareMaterial(const NativeSunFlareMaterial& h)noexcept {
    if(h.parameters08){const auto& b=*h.parameters08;
        if(!b.packetStarted){
            if(b.packetKind!=NativeSunFlareParameters::PacketKind::none||b.packet||b.buffer20||b.nativePayload04||b.packetComplete||b.count24||b.count28||b.field2C!=-1)return false;
            for(const auto& word:b.unknown00To1C)if(word)return false;
        }else{
            if(b.packetKind==NativeSunFlareParameters::PacketKind::none||b.field2C!=1||b.count24!=128)return false;
            if(!b.packet){
                if(b.buffer20||b.nativePayload04||b.packetComplete||b.count28)return false;
                for(const auto& word:b.unknown00To1C)if(!word||*word)return false;
            }else{
                const bool sprite=b.packetKind==NativeSunFlareParameters::PacketKind::sprite;
                if(b.buffer20!=b.packet->words.data()||!b.packetComplete||b.count28!=(sprite?41u:1u)||
                   b.nativePayload04!=b.buffer20||b.unknown00To1C[0]!=(sprite?0x3000000bu:0x30000001u)||b.unknown00To1C[1])return false;
                for(std::size_t i=2;i<8;++i)if(b.unknown00To1C[i]!=0u)return false;
                if(sprite){
                    if(b.packet->words[3]!=0x6c098320u||b.packet->words[40]!=0x14000000u||
                       b.packet->words[41]||b.packet->words[42]||b.packet->words[43])return false;
                }else for(std::size_t i=0;i<4;++i)if(b.packet->words[i])return false;
            }
        }
    }
    if(!h.primitive04)return true;
    const auto& p=*h.primitive04;
    if(!h.parameters08||p.optional14||(p.flags0F&1u)||
       (p.parameters10&&p.parameters10!=h.parameters08.get()))return false;
    if(p.optional04&&(!p.configured||p.optional04->sourceFunction!=0x2c5610||!p.optional04->self))return false;
    if(p.configured&&(!p.retained||p.type0C!=15||p.parameters10!=h.parameters08.get()))return false;
    if(!p.retained)return true; //Allocated or row-stored failed prefix owns no numeric ref.
    rr::assets::OriginalStandardMaterialRowView50LA row;
    return p.stored&&p.table&&p.material00>=0&&p.table->canRelease14D500(p.material00)&&p.table->row(p.material00,row)&&
        row.references08&&*row.references08>0;
}
inline void Owner::retireNativeSunFlareMaterial(NativeSunFlareMaterial& h)noexcept {
    if(!canRetireNativeSunFlareMaterial(h))std::terminate();
    //245D10: primitive04 (246260->299AC0 release14D500) BEFORE parameters08.
    //246260 does not own borrowed parameters10: flags0F bit0 was cleared.
    if(h.primitive04&&h.primitive04->retained){
        if(!h.primitive04->table->release14D500(h.primitive04->material00))std::terminate();
        h.primitive04->retained=false;
    }
    h.primitive04.reset();h.parameters08.reset(); //133FA0 frees actual aligned buffer20 if reached.
}
inline bool Owner::canRetireNativeSunFlare()const noexcept {
    if(!nativeSunFlares_)return true;
    const auto& d=*nativeSunFlares_;
    if(!worldApplication_||worldApplication_->attachmentLock423F58()||
       d.generation!=worldEntity_.sessionGeneration||!d.count4A8||*d.count4A8!=1)return false;
    if(!d.owner)return !d.pointers4A4||!d.pointers4A4[0];
    const auto& o=*d.owner;const auto& e=o.entity;
    if((o.sprite64&&!canRetireNativeSunFlareMaterial(o.sprite64->material))||
       !canRetireNativeSunFlareMaterial(o.material68)||(o.callback74&&o.callback74!=&o))return false;
    if(o.sprite64&&o.sprite64->material.primitive04&&o.sprite64->material.primitive04->optional04)return false;
    if(o.material68.primitive04&&o.material68.primitive04->optional04&&
       (o.material68.primitive04->optional04->self!=&o||o.callback74!=&o))return false;
    if((o.colors44||o.colors48||o.buffer78)&&o.zones30!=10)return false;
    if(o.colorRowsInitialized>10||o.colorsInitialized!=(o.colorRowsInitialized==10)||
       (o.colorRowsInitialized&&(!o.colors44||!o.colors48||!o.depth40||!(o.flags98KnownValue&2u))))return false;
    return d.pointers4A4&&d.pointers4A4[0]==&o&&o.counted&&
        e.reference00==1&&e.kind==g::OriginalWreckComponentKind50LA::entity141300&&
        e.objectKey==key(&e)&&e.sessionGeneration==d.generation&&e.category28==0xff&&
        (e.flags29&15u)==3u&&!e.nativeFlags29&&!e.parent1C&&!e.firstChild18&&!e.previous10&&!e.next14&&
        !e.mask20&&!e.descendants24&&worldApplication_->objectCount423F60()&&
        worldApplication_->entityAuthority_.canRetire(e);
}
inline bool Owner::retireNativeSunFlare()noexcept {
    if(!canRetireNativeSunFlare())return false;
    if(!nativeSunFlares_)return true;
    auto& d=*nativeSunFlares_;
    if(d.owner){auto& o=*d.owner;
        //2C58C0 releases44,48,78 in this order, then sprite64.
        o.colors44.reset();o.colors48.reset();o.buffer78.reset();
        //Sprite64's virtual destructor
        //2C59B0 calls245D10, then embedded material68 calls245D10.
        //Preserve exact numeric-reference release before Entity1413E0.
        if(o.sprite64){retireNativeSunFlareMaterial(o.sprite64->material);o.sprite64.reset();}
        retireNativeSunFlareMaterial(o.material68);
        //Retail270110 frees the table, not this detached object; do NOT call
        //that a full retail World destructor or invent a missing parent.
        if(!worldApplication_->entityAuthority_.retire(o.entity))std::terminate();
        --o.entity.reference00;--worldApplication_->objectCount423F60();o.counted=false;
        d.pointers4A4[0]=nullptr;d.owner.reset();
    }
    d.pointers4A4.reset();nativeSunFlares_.reset();return true;
}
}

namespace ra_pod_scene50LA {
inline const native_world_water50LA::Domain* Owner::readNativeAbsentWater400(std::uint32_t& pc)const noexcept {
    pc=0x1aa400;
    if(!nativeWaterDomain_||!worldColdConstructed_||!worldApplication_||
       worldApplication_->attachmentLock423F58()||worldEntity_.objectKey!=key(&worldEntity_)||
       worldEntity_.reference00!=1u||!worldEntity_.sessionGeneration||
       !nativeWaterDomain_->readAbsent(*worldApplication_,worldEntity_,worldEntity_.sessionGeneration))return nullptr;
    //Typed domain identity, NOT a Water object pointer or guest400 bit pattern.
    //No caller copy/publication is enabled by this private observation leaf.
    return nativeWaterDomain_.get();
}
}
namespace ra_cold_aggregate50LA {
inline bool Owner::nativeWaterDomainMatches(const native_world_water50LA::Domain& domain)const noexcept {
    return !closed_&&!terminal_&&worldColdConstructed_&&worldApplication_&&source_&&
        source_->sourcesDecoded()&&nativeWaterDomain_.get()==&domain&&
        !worldApplication_->attachmentLock423F58()&&worldEntity_.reference00==1u&&
        worldEntity_.objectKey==key(&worldEntity_)&&worldEntity_.sessionGeneration&&
        domain.matches(*worldApplication_,worldEntity_,worldEntity_.sessionGeneration,
            source_->sourcesDecoded()->height);
}
}

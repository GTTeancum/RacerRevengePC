#pragma once
#include "original_ui_main_entry50kg.h"
#include "ra_cold_aggregate50la.h"
#include "original_source_world_trigger_instance_custody50la.inl"
#include "original_source_world_trigger_action_custody50la.inl"
#include "original_source_world_trigger_custody50la.inl"
#include "original_source_world_toy_custody50la.inl"
#include "original_source_world_sunflare_custody50la.inl"
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

inline bool Owner::reserveSourceCollisionAuthorityNative()noexcept {
    return worldApplication_&&worldApplication_->application10()&&!worldApplication_->attachmentLock423F58()&&
        worldApplication_->objectCount423F60()<=std::numeric_limits<std::uint32_t>::max()-10u&&
        worldApplication_->entityAuthority_.reserveAdditional(10);
}
inline bool Owner::constructSourceCollisionGroup27BB40(std::size_t i,std::uint32_t& pc)noexcept {
    pc=0x27bb4c;if(!worldCollisionObjects3CC_||!worldCollisionObjects3CC_->sourceMode_||i>=10||
       !worldCollisionObjects3CC_->sourceZones_[i]||!worldApplication_)return false;
    auto& z=*worldCollisionObjects3CC_->sourceZones_[i];auto& a=worldApplication_->entityAuthority_;
    if(z.counted||z.sourceEnrolled||!a.canEnroll(z.entity,key(&z.entity)))return false;
    const auto r=g::initializeOriginalWreckComponent50LA(z.entity,g::OriginalWreckComponentKind50LA::collisionObjects27BB40,
        worldEntity_.sessionGeneration,key(&z.entity),true,worldApplication_->objectCount423F60());
    if(r.status!=g::OriginalWreckComponentStatus50LA::complete){pc=r.sourcePC;return false;}
    z.counted=true; //Reached reference/count survives any later native refusal.
    if(!a.enroll(z.entity)){pc=0x27bb54;return false;}z.sourceEnrolled=true;
    pc=0x27bb5c;z.sourceVtable08=0x3d25b0;
    pc=0x27bb60;z.count30=0;z.sourceCountKnown=true;
    pc=0x27bb64;z.sourceEntries34.reset();return true;
}

inline bool Owner::prepareWorldCollisionObjects271230(
    std::shared_ptr<const ra_cold_sources50LA::Owner> source,std::uint32_t& pc)noexcept {
    pc=0;
    if(!worldColdConstructed_||!worldList500_||!worldPhysics3D4_||worldCollisionObjects3CC_||sourceCollisionObjectsAttempted_||
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
    if(objects.sourceMode_){
        if(objects.source_||worldCollisionResources_||nativeCollisionModelBorrow_||
           objects.sourceArrayKnown_>10)return fail(pc,0x27bb80);
        for(const auto& legacy:objects.zones_)if(legacy.counted||!legacy.entries34.empty())return fail(pc,0x27bb80);
        if(objects.sourceArrayReturned_){if(!sourceCollisionObjects3CC_||*sourceCollisionObjects3CC_!=objects.sourceArray3CC_.get())return fail(pc,0x1b4efc);}
        else if(sourceCollisionObjects3CC_)return fail(pc,0x1b4ef4);
        if(!objects.sourceArray3CC_&&objects.sourceArrayKnown_)return fail(pc,0x1b4f3c);
        std::size_t counted=0;
        for(std::size_t i=0;i<10;++i){const auto& p=objects.sourceZones_[i];
            if(i<objects.sourceArrayKnown_&&objects.sourceArray3CC_[i]!=(p?&p->entity:nullptr))return fail(pc,0x1b4f3c);
            if(!p){for(const auto& w:objects.sourceWrappers_[i])if(w)return fail(pc,0x27bb80);continue;}
            const auto& z=*p;const auto& e=z.entity;
            if(!z.sourceMode||!z.entries34.empty()||z.sourceEntriesKnown>objects.sourceWrappers_[i].size()||
               (!z.sourceEntries34&&z.sourceEntriesKnown)||
               (z.sourceArrayReturned&&(!z.sourceCountKnown||z.count30!=objects.sourceWrappers_[i].size()))||
               (z.sourceEntries34&&!z.sourceArrayReturned))return fail(pc,0x27bb80);
            if(z.counted){
                if(e.reference00!=1||e.previous10||e.next14||e.parent1C||e.firstChild18||e.mask20||e.descendants24||
                   e.objectKey!=key(&e)||e.sessionGeneration!=worldEntity_.sessionGeneration)return fail(pc,0x1413e0);
                if(z.sourceEnrolled){if(!worldApplication_->entityAuthority_.canRetire(e))return fail(pc,0x1413e0);}
                else { //Only exact failed native enrollment; never retire an unacquired row.
                    for(std::size_t n=0;n<worldApplication_->entityAuthority_.count_;++n)
                        if(worldApplication_->entityAuthority_.entries_[n].node==&e)return fail(pc,0x1413e0);
                }
                ++counted;
            }else if(z.sourceEnrolled||e.reference00)return fail(pc,0x1413e0);
            for(std::size_t n=0;n<objects.sourceWrappers_[i].size();++n){const auto& w=objects.sourceWrappers_[i][n];
                if(n<z.sourceEntriesKnown&&z.sourceEntries34[n]!=w.get())return fail(pc,0x27bbf4);
                if(w&&(!w->sourceMode||w->collision04||w->nativeQuery||w->decodedCollision||w->source||
                    w->descriptorRetired||(w->descriptor08&&!w->descriptor08->sourceFields)))return fail(pc,0x27bbf4);
            }
        }
        if(worldApplication_->objectCount423F60()<counted)return fail(pc,0x1413e0);
        return true;
    }
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
    if(objects.sourceMode_){
        //Read-only preflight above closes every refusal gate. No callbacks or
        //source destructor replay follow: raw unknown tails are never read.
        sourceCollisionObjects3CC_.reset(); //Native withdrawal, not guestNULL.
        for(std::size_t i=10;i-->0;){auto& p=objects.sourceZones_[i];
            for(std::size_t n=objects.sourceWrappers_[i].size();n-->0;)objects.sourceWrappers_[i][n].reset();
            if(!p)continue;p->sourceEntries34.reset();
            if(p->counted){
                if(p->sourceEnrolled&&!worldApplication_->entityAuthority_.retire(p->entity))std::terminate();
                p->sourceEnrolled=false;--p->entity.reference00;
                --worldApplication_->objectCount423F60();p->counted=false;
            }
            p.reset();
        }
        objects.sourceArray3CC_.reset();worldCollisionObjects3CC_.reset();return true;
    }

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
    if(!worldColdConstructed_||!worldCollisionObjects3CC_||worldCollisionObjects3CC_->sourceMode_||worldCollisionResources_||topologyAttempted_||
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
    std::uint64_t generation,const track_height_cold50JB::Owner& height,std::uint32_t& pc,
    ra_cold_aggregate50LA::Owner& concrete)noexcept {
    pc=0;
    if(static_cast<ra_pod_scene50LA::Owner*>(&concrete)!=this||
       worldColdAttempted_||nativeWaterDomain_||topologyAttempted_||topologyClosed_||!app||!app->application10()||
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
    worldColdAttempted_=true;worldApplication_=std::move(app);worldAudio_=&audio;
    const auto r=g::initializeOriginalWreckComponent50LA(worldEntity_,
        g::OriginalWreckComponentKind50LA::entity141300,generation,worldKey,true,
        worldApplication_->objectCount423F60());
    if(r.status!=g::OriginalWreckComponentStatus50LA::complete)std::terminate();
    if(!worldApplication_->entityAuthority_.enroll(worldEntity_))std::terminate();
    worldColdConstructed_=true;worldMode30_=1; //26FCBC before actual270550 reset.
    //26FCCC actual base-vtable78=270550, ONLY during fresh construction.
    //No loaded-world reset;3CC/3D4/4D8 are NOT written by this helper.
    if(!concrete.resetFreshWorld270550(pc))return false;
    if(!audio.constructNextCold2350D0(mixer,worldApplication_,generation,g::RaceAudioGroup50IC::World4D8))
        return fail(pc,0x26fcf8); //Retain world prefix; explicit native rollback below.
    //1B28A4 list allocation follows actual270550 and World4D8 construction.
    //Failure must retain those counted/source prefixes, not preflight them away.
    try {list.reset(new Entity*[10]);}catch(...){return fail(pc,0x1b5f64);}
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
inline bool Owner::canRetireNativeWorldPrefix(const char** gate,std::size_t extraSourceOwnedCount)const noexcept {
    const auto reject=[&](const char* text){if(gate)*gate=text;return false;};
    if(constructorRacerWorldBorrows_)return reject("live constructor Racer World source borrow");
    if(!worldColdConstructed_||!worldApplication_||!worldAudio_||
       worldApplication_->attachmentLock423F58())return reject("world prefix actual owner/lock");
    const auto& e=worldEntity_;const auto& audio=*worldAudio_;
    if(e.reference00!=1||e.previous10||e.next14||e.parent1C||e.firstChild18||e.mask20||e.descendants24||
       e.objectKey!=key(&e)||audio.coldApplication_!=worldApplication_||
       audio.coldSession_!=e.sessionGeneration||audio.coldThread_!=std::this_thread::get_id()||
       !audio.coldMixer_||audio.coldEpoch_!=audio.coldMixer_->groupEpoch50IC())return reject("world prefix Entity/audio identity");
    if(!worldApplication_->entityAuthority_.canRetire(e))return reject("world prefix Entity authority");
    if(sourceToyAttempted_&&!canRetireSourceToyNative())return reject("source Toy3C8 counted custody");
    if(sourceTriggerAttempted_&&!canRetireSourceTriggerNative())return reject("source Trigger3D0 counted custody");
    if(nativeSunFlares_&&nativeSunFlares_->sourceMode&&!canRetireSourceSunFlareNative())return reject("source SunFlare4A4 counted custody");
    if(nativeWaterDomain_&&!nativeWaterDomain_->canRetire(*worldApplication_,e,e.sessionGeneration))
        return reject("native water constructor domain identity");
    if(worldList500_&&worldListCount50C_)return reject("world prefix nonempty list500");
    if(!canRetireNativeCollisionQueries27B040())return reject("world collision query/resource custody");
    std::uint32_t pc=0;if(!canReleaseWorldCollisionObjects27BB80(pc))return reject("world collision object topology");
    if(worldPhysics3D4_&&!worldPhysics3D4_->canRelease27C030())return reject("world physics index references");
    const auto& group=audio.coldGroups_[static_cast<std::size_t>(g::RaceAudioGroup50IC::World4D8)];
    if(group.constructed&&!audio.canReleaseEmptyCold142590(g::RaceAudioGroup50IC::World4D8))return reject("world seventh audio group retirement");
    const std::size_t required=1u+std::size_t(group.constructed)+sourceTriggerInstanceCountNative()+
        (nativeSunFlares_&&nativeSunFlares_->sourceMode&&nativeSunFlares_->owner&&nativeSunFlares_->owner->counted?1u:0u)+
        (nativeTriggerList3D0_&&nativeTriggerList3D0_->sourceMode&&nativeTriggerList3D0_->counted?1u:0u)+
        (nativeEmptyToyList3C8_&&nativeEmptyToyList3C8_->sourceMode&&nativeEmptyToyList3C8_->sourceCounted?1u:0u)+
        (worldCollisionObjects3CC_?(worldCollisionObjects3CC_->sourceMode_?worldCollisionObjects3CC_->sourceCountedGroups():worldCollisionObjects3CC_->zones_.size()):0u);
    if(extraSourceOwnedCount>std::numeric_limits<std::size_t>::max()-required||
       worldApplication_->objectCount423F60()<required+extraSourceOwnedCount)return reject("world prefix Application count");
    return true;
}
inline bool Owner::retireNativeWorldPrefixAfterOwnedCleanup(std::uint32_t& pc)noexcept {
    pc=0;
    if((sourceTopologyMode_&&!sourceTopologyWithdrawn_)||
       (nativeSunFlares_&&nativeSunFlares_->sourceMode&&!nativeSunFlares_->sourceNativeClosed)||
       !canRetireNativeWorldPrefix()||(sourceTriggerAttempted_&&nativeTriggerList3D0_)||(sourceToyAttempted_&&nativeEmptyToyList3C8_)||runtimeQueries_||trackParticles_||environmentMethod_||
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
    if(nativeSunFlares_&&nativeSunFlares_->sourceMode&&!nativeSunFlares_->sourceNativeClosed)return fail(pc,0x1b2870);
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
    if(sourceTrackNativeAttempted_||closed_||terminal_||!candidates_||!worldColdConstructed_||app.entities()!=worldApplication_||
       runtimeCameraBorrows_||runtimeWorldBorrows_)return false;
    try {
        const auto domain=app.standardMaterialDomain();
        if(!domain||!domain->prepared())return false;
        return ra_pod_scene50LA::Owner::prepareWorldCollisionResources(candidates_,domain->resolver3E1A70(),d);
    }catch(...){return false;}
}
inline bool Owner::resetFreshWorld270550(std::uint32_t& pc)noexcept {
    pc=worldReset270550PC_;
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(worldReset270550Attempted_)return false; //Never replay on loaded/failed World.
    if(!worldColdConstructed_||!worldApplication_||!worldAudio_||worldMode30_!=1||
       topologyAttempted_||topologyClosed_||nativeWaterDomain_||
       worldApplication_->attachmentLock423F58()||worldEntity_.reference00!=1||
       !worldApplication_->entityAuthority_.canRetire(worldEntity_)||
       nativeScb34_||nativeTob34_||nativeAub38_||sourceWorldAudioConfig_||sourceAudioPhase_!=SourceAudioPhase::unreached||sourceAmbientPhase_!=SourceAmbientPhase::unreached||sourceSoulTreeSoundPhase_!=SourceSoulTreeSoundPhase::unreached||sourceMaterialSoundPhase_!=SourceMaterialSoundPhase::unreached||nativePortalParameters3A0_||
       sourceToy3C8_||sourceTerrain3B0_||sourcePortal3A0_||sourceLight3A8_||sourceFog3AC_||nativeLightList3A8_||nativeFogDescriptor3AC_||nativeTerrain3B0_||nativeEmptyToyList3C8_||
       nativeParticleList3D8_||nativeEmptyInstances3DC_||nativeFog3EC_||nativeFogOn3F0_||
       nativeFogOff3F4_||nativeSky3F8_||nativeParticleManager4A0_||nativeTriggerList3D0_||
       nativeCsCameraTable3E4_||nativeSunFlares_||nativeMethods4C8_||nativeMethods4CC_||
       nativeMethods4D0_||nativeMethods4D4_||nativeEmptyAmbient3B4_||nativeSoulTreeSounds3B8_||
       nativeMaterialSounds3BC_[0]||nativeMaterialSounds3BC_[1])return false;
    worldReset270550Attempted_=true;
    const auto at=[&](std::uint32_t site){pc=worldReset270550PC_=site;};
    //All writes touch authoritative source publications or their existing
    //actual owners. Immutable/prewarmed backing is NEVER destroyed here.
    at(0x270550);nativeScb34_.reset();nativeTob34_.reset();worldParser34_=nullptr;
    at(0x270554);nativeAub38_.reset();sourceAudioParser38_=nullptr;
    at(0x270558);worldText3C_[0]=0;
    at(0x27055c);worldText140_[0]=0;
    at(0x270560);worldText244_[0]=0;
    at(0x270564);worldBoundary348_=nullptr;
    at(0x270568);worldZones34C_=0;
    at(0x27056c);worldZoneInfo350_=nullptr;
    at(0x270570);nativeModelZone354_=0;
    at(0x270574);worldPortals358_=0;
    at(0x270578);useEnvironment35C_=0;
    at(0x270590);world390KnownValue_&=0xfeu;world390KnownMask_|=1u;
    at(0x270594);nativePortalParameters3A0_.reset();sourcePortal3A0_=nullptr;
    at(0x270598);nativeEnvironment3A4_=nullptr;
    at(0x27059c);nativeLightList3A8_.reset();sourceLight3A8_=nullptr;
    at(0x2705a0);nativeFogDescriptor3AC_.reset();sourceFog3AC_=nullptr;
    at(0x2705a4);nativeTerrain3B0_.reset();sourceTerrain3B0_=nullptr;
    at(0x2705a8);if(!initializeModels3C42705A8())return false;
    at(0x2705ac);nativeEmptyToyList3C8_.reset();sourceToy3C8_=nullptr;
    at(0x2705b0);nativeParticleList3D8_.reset();sourceParticleList3D8_=nullptr;
    at(0x2705b4);nativeInstanceDescriptors3DC_=nullptr;
    at(0x2705b8);nativeInstanceCount3E0_=0;
    at(0x2705bc);nativeFog3EC_.reset();
    at(0x2705c0);nativeFogOn3F0_.reset();
    at(0x2705c4);nativeFogOff3F4_.reset();
    at(0x2705c8);nativeSky3F8_.reset();
    at(0x2705cc);worldWater3FC_=nullptr;
    at(0x2705d0);worldWater400_=nullptr;
    at(0x2705d4);worldWater404_=nullptr;
    at(0x2705d8);worldEcosystem408_=nullptr;
    at(0x2705dc);nativeParticleManager4A0_=nullptr;
    at(0x2705e0);worldToyManager49C_=nullptr;
    at(0x2705e4);nativeTriggerList3D0_.reset();
    at(0x2705e8);nativeCsCameraCount3E4_=0;
    at(0x2705ec);nativeCsCameraZones3E8_=nullptr;
    at(0x2705f0);nativeSunFlarePointers4A4_=nullptr; //4A8 untouched/unknown.
    at(0x2705f4);worldPhysicsObjectCount4B0_=0;
    at(0x2705f8);worldPhysicsObjectPointers4B4_=nullptr;
    at(0x2705fc);worldRole4B8_=nullptr;
    at(0x270600);if(!initializeTopology4BC270600())return false;
    at(0x270604);if(!initializeTopology4C0270604())return false;
    at(0x270608);if(!initializeTopology4C4270608())return false;
    at(0x27060c);nativeMethods4C8_.reset();
    at(0x270610);nativeMethods4CC_.reset();
    at(0x270614);nativeMethods4D0_.reset();
    at(0x270618);nativeMethods4D4_.reset();
    at(0x27061c);worldRoles4DCTo4EC_[0]=nullptr;
    at(0x270620);worldRoles4DCTo4EC_[1]=nullptr;
    at(0x270624);worldRoles4DCTo4EC_[2]=nullptr;
    at(0x270628);worldRoles4DCTo4EC_[3]=nullptr;
    at(0x27062c);worldRoles4DCTo4EC_[4]=nullptr;
    at(0x27063c);world4F0KnownValue_&=0xfeu;world4F0KnownMask_|=1u;
    at(0x270640);nativeEmptyAmbient3B4_.reset();sourceAmbient3B4_=nullptr;
    at(0x270644);nativeSoulTreeSounds3B8_.reset();sourceSoulTreeSounds3B8_=nullptr;
    at(0x270648);nativeMaterialSounds3BC_[0].reset();sourceMaterialSounds3BC_[0]=nullptr;
    at(0x270650);nativeMaterialSounds3BC_[1].reset();sourceMaterialSounds3BC_[1]=nullptr; //JR27064C delay-slot store.
    worldReset270550Complete_=true;return true;
}

inline bool Owner::resetColdLoadWorld270550(std::uint32_t& pc)noexcept {
    using Phase=SourceLoadReset270550Phase;
    pc=sourceLoadReset270550PC_;
    // Private caller-issued invocation, not constructor reset replay. Predicate
    // is defined beside the actual complete Race type and verifies same owner.
    if(nativeScbThread_!=std::this_thread::get_id()||
       sourceLoadReset270550Phase_!=Phase::invoked1AA040||!sourceLoadRace_||!sourceLoadApplication_||
       !sourceWorldLoadInvocationCurrent()||closed_||terminal_)return false;
    if(!worldColdConstructed_||!worldReset270550Attempted_||!worldReset270550Complete_||
       !worldApplication_||!worldAudio_||worldMode30_!=1||
       worldApplication_->attachmentLock423F58()||worldEntity_.reference00!=1||
       worldEntity_.objectKey!=key(&worldEntity_)||!worldEntity_.sessionGeneration||
       !worldApplication_->entityAuthority_.canRetire(worldEntity_)||
       worldEntity_.parent1C||worldEntity_.previous10||worldEntity_.next14||worldEntity_.firstChild18||
       constructorRacerWorldBorrows_||runtimeCameraBorrows_||runtimeWorldBorrows_||
       sourceTopologyMode_||topologyAttempted_||topologyConstructed_||topologyClosed_||sourceTopologyWithdrawn_||
       sourceVisibilityPublished_||sourceCollisionPublished_||domainCount_||nativeTopologyAuthorityEnrolled_||
       worldPhysics3D4_||worldCollisionObjects3CC_||worldCollisionResources_||livePhysicsIndexAttempted_||
       restartWorldPhysicsAttempted_||nativeCollisionPrefixEntered_||nativeCollisionModelBorrow_||
       modelPrefixesAttempted_||modelParents3C4Attempted_||sourceModelParentsPublished3C4_||
       sourceModelParents3C4_||sourceModelOwner3C4_||trackParticleAttempted_||particlePredecessorAttempted_||
       particlesAttempted_||worldAttempted_||worldImagesAttempted_||worldDrawPlanAttempted_||
       nativeScbRevoked_||nativeScb34_||nativeTob34_||nativeAub38_||sourceWorldAudioConfig_||sourceAudioPhase_!=SourceAudioPhase::unreached||sourceAmbientPhase_!=SourceAmbientPhase::unreached||sourceSoulTreeSoundPhase_!=SourceSoulTreeSoundPhase::unreached||sourceMaterialSoundPhase_!=SourceMaterialSoundPhase::unreached||nativeSceneInfo270900_||
       nativePortalParameters3A0_||nativeLightList3A8_||nativeFogDescriptor3AC_||nativeTerrain3B0_||
       nativeEmptyToyList3C8_||nativeParticleList3D8_||nativeEmptyInstances3DC_||
       nativeFog3EC_||nativeFogOn3F0_||nativeFogOff3F4_||nativeFogAttempted_||nativeSky3F8_||
       nativeWaterNone274570_||nativeParticleManager4A0_||nativeTriggerList3D0_||
       nativeCsCameraTable3E4_||nativeSunFlares_||nativeFinishLine540_||
       nativeMethods4C8_||nativeMethods4CC_||nativeMethods4D0_||nativeMethods4D4_||
       nativeEmptyAmbient3B4_||nativeSoulTreeSounds3B8_||nativeMaterialSounds3BC_[0]||nativeMaterialSounds3BC_[1]||
       environmentMethod_||environment443108_.constructed||method34Retained_||method38Retained_||
       nativeWreckZoneBorrower_||sourceVisibilitySlots4C0_||sourceCollisionSlots4C4_||container4BC_.reference00)
        return false;
    // The genuine constructor append-list remains allocated and empty. Never
    // inspect uninitialized unused pointer slots or clear/free its backing.
    if(!worldList500_||worldListCount50C_||worldListCapacity504_!=10u||worldListGrowth508_!=100000u)return false;
    for(const auto& portal:nativePortals_)if(portal)return false;
    for(const auto& node:visibility4C0_)if(node.constructed||node.attached||node.entity.reference00||node.exclusions3C)return false;
    for(const auto& node:collision4C4_)if(node.constructed||node.attached||node.entity.reference00||node.exclusions3C)return false;
    // Unlike the constructor's pre-domain call, an actual never-bound native
    // WaterDomain already exists. It is NOT source400 and must not be recreated,
    // revoked or re-labelled by this source reset.
    if(!nativeWaterDomain_||!nativeWaterDomain_->identity(*worldApplication_,worldEntity_,worldEntity_.sessionGeneration)||
       nativeWaterDomain_->everBound_||nativeWaterDomain_->context_||nativeWaterDomain_->predicate_||
       nativeWaterDomain_->reading_||nativeWaterDomain_->currentnessLost_||nativeWaterDomain_->epoch_)return false;
    const auto empty=[](const auto& value)noexcept{return value.has_value()&&*value==nullptr;};
    if(!empty(sourceMaterialSounds3BC_[0])||!empty(sourceMaterialSounds3BC_[1])||!empty(sourceSoulTreeSounds3B8_)||!empty(sourceAmbient3B4_)||!empty(sourceAudioParser38_)||!empty(worldParser34_)||!empty(worldBoundary348_)||!empty(worldZoneInfo350_)||
       !empty(sourceParticleList3D8_)||!empty(sourceToy3C8_)||!empty(sourceTerrain3B0_)||!empty(sourcePortal3A0_)||!empty(nativeEnvironment3A4_)||!empty(sourceLight3A8_)||!empty(sourceFog3AC_)||!empty(sourceModels3C4_)||!empty(nativeInstanceDescriptors3DC_)||
       !empty(worldWater3FC_)||!empty(worldWater400_)||!empty(worldWater404_)||!empty(worldEcosystem408_)||
       !empty(worldToyManager49C_)||!empty(nativeCsCameraZones3E8_)||!empty(nativeSunFlarePointers4A4_)||
       !empty(worldPhysicsObjectPointers4B4_)||!empty(worldRole4B8_)||!empty(sourceContainer4BC_)||
       !empty(sourceVisibility4C0_)||!empty(sourceCollision4C4_))return false;
    for(const auto& role:worldRoles4DCTo4EC_)if(!empty(role))return false;
    sourceLoadReset270550Phase_=Phase::entered;
    const auto at=[&](std::uint32_t site)noexcept {pc=sourceLoadReset270550PC_=site;};
    // Distinct exact-empty repeat stores. The strict fresh constructor helpers
    // remain unchanged and continue rejecting already-engaged optional lanes.
    const auto repeatNull=[&](auto& lane,std::uint32_t site)noexcept {
        at(site);if(!empty(lane)){sourceLoadReset270550Phase_=Phase::failed;return false;}
        lane=nullptr;return true;
    };
    // The exact original reset stores repeat only already-empty publications.
    // Native immutable owners and the existing WaterDomain remain untouched.
    at(0x270550);nativeScb34_.reset();nativeTob34_.reset();worldParser34_=nullptr;
    at(0x270554);nativeAub38_.reset();sourceAudioParser38_=nullptr;
    at(0x270558);worldText3C_[0]=0;
    at(0x27055c);worldText140_[0]=0;
    at(0x270560);worldText244_[0]=0;
    at(0x270564);worldBoundary348_=nullptr;
    at(0x270568);worldZones34C_=0;
    at(0x27056c);worldZoneInfo350_=nullptr;
    at(0x270570);nativeModelZone354_=0;
    at(0x270574);worldPortals358_=0;
    at(0x270578);useEnvironment35C_=0;
    at(0x270590);world390KnownValue_&=0xfeu;world390KnownMask_|=1u;
    at(0x270594);nativePortalParameters3A0_.reset();sourcePortal3A0_=nullptr;
    at(0x270598);nativeEnvironment3A4_=nullptr;
    at(0x27059c);nativeLightList3A8_.reset();sourceLight3A8_=nullptr;
    at(0x2705a0);nativeFogDescriptor3AC_.reset();sourceFog3AC_=nullptr;
    at(0x2705a4);nativeTerrain3B0_.reset();sourceTerrain3B0_=nullptr;
    if(!repeatNull(sourceModels3C4_,0x2705a8))return false;
    at(0x2705ac);nativeEmptyToyList3C8_.reset();sourceToy3C8_=nullptr;
    at(0x2705b0);nativeParticleList3D8_.reset();sourceParticleList3D8_=nullptr;
    at(0x2705b4);nativeInstanceDescriptors3DC_=nullptr;
    at(0x2705b8);nativeInstanceCount3E0_=0;
    at(0x2705bc);nativeFog3EC_.reset();
    at(0x2705c0);nativeFogOn3F0_.reset();
    at(0x2705c4);nativeFogOff3F4_.reset();
    at(0x2705c8);nativeSky3F8_.reset();
    at(0x2705cc);worldWater3FC_=nullptr;
    at(0x2705d0);worldWater400_=nullptr;
    at(0x2705d4);worldWater404_=nullptr;
    at(0x2705d8);worldEcosystem408_=nullptr;
    at(0x2705dc);nativeParticleManager4A0_=nullptr;
    at(0x2705e0);worldToyManager49C_=nullptr;
    at(0x2705e4);nativeTriggerList3D0_.reset();
    at(0x2705e8);nativeCsCameraCount3E4_=0;
    at(0x2705ec);nativeCsCameraZones3E8_=nullptr;
    at(0x2705f0);nativeSunFlarePointers4A4_=nullptr; //4A8 untouched/unknown.
    at(0x2705f4);worldPhysicsObjectCount4B0_=0;
    at(0x2705f8);worldPhysicsObjectPointers4B4_=nullptr;
    at(0x2705fc);worldRole4B8_=nullptr;
    if(!repeatNull(sourceContainer4BC_,0x270600))return false;
    if(!repeatNull(sourceVisibility4C0_,0x270604))return false;
    if(!repeatNull(sourceCollision4C4_,0x270608))return false;
    at(0x27060c);nativeMethods4C8_.reset();
    at(0x270610);nativeMethods4CC_.reset();
    at(0x270614);nativeMethods4D0_.reset();
    at(0x270618);nativeMethods4D4_.reset();
    at(0x27061c);worldRoles4DCTo4EC_[0]=nullptr;
    at(0x270620);worldRoles4DCTo4EC_[1]=nullptr;
    at(0x270624);worldRoles4DCTo4EC_[2]=nullptr;
    at(0x270628);worldRoles4DCTo4EC_[3]=nullptr;
    at(0x27062c);worldRoles4DCTo4EC_[4]=nullptr;
    at(0x27063c);world4F0KnownValue_&=0xfeu;world4F0KnownMask_|=1u;
    at(0x270640);nativeEmptyAmbient3B4_.reset();sourceAmbient3B4_=nullptr;
    at(0x270644);nativeSoulTreeSounds3B8_.reset();sourceSoulTreeSounds3B8_=nullptr;
    at(0x270648);nativeMaterialSounds3BC_[0].reset();sourceMaterialSounds3BC_[0]=nullptr;
    at(0x270650);nativeMaterialSounds3BC_[1].reset();sourceMaterialSounds3BC_[1]=nullptr; //JR27064C delay-slot store.
    sourceLoadReset270550Phase_=Phase::returned;return true;
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
        source_->sourcesDecoded()->height,pc,*this);
}
}

namespace ra_pod_scene50LA {
inline bool Owner::loadNativeEmptyAnimations2713E0(
    const rr::assets::ParameterDocument48F& document,std::uint32_t& pc)noexcept {
    pc=0x271d34;
    if(!worldColdConstructed_||!worldApplication_||!worldAudio_||
       worldAudio_->coldThread_!=std::this_thread::get_id()||
       worldApplication_->attachmentLock423F58()||sourceToyAttempted_||nativeEmptyToyList3C8_)return false;
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
    if(sourceTriggerAttempted_)return false;
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
    if(!nativeTriggerList3D0_||nativeTriggerList3D0_->sourceMode||i>=2u||!worldApplication_||worldApplication_->attachmentLock423F58())return false;
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
    //Actual descriptor00 publication occurs AFTER trigger constructor and
    //BEFORE collider/configuration2A04B0. No second trigger allocation.
    pc=0x2dbffc;
    if(!list.descriptors34||i>=list.descriptorSlotsPublished||!list.descriptors34[i])return false;
    list.descriptors34[i]->trigger00=&pair.trigger;
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

inline bool Owner::nativeTriggerDescriptorCurrent(std::size_t i)const noexcept {
    if(!nativeTriggerList3D0_||nativeTriggerList3D0_->sourceMode)return false;const auto& list=*nativeTriggerList3D0_;
    if(list.descriptorsRetired||i>=list.descriptorSlotsPublished||i>=2u||!list.descriptors34||
       !list.authoring||!list.authoring->sources()||!list.actions)return false;
    const auto* d=list.descriptors34[i];
    return d&&d==list.descriptorOwners[i].get()&&d->complete&&
        d->authoring==&(*list.authoring->sources())[i]&&d->trigger00==&list.actions->nativeEntities_[i].trigger&&
        d->actionCount04==1&&d->actions68&&d->actionSlotsPublished==1&&
        d->action&&d->actions68[0]==d->action.get()&&list.actions->nativeActionDescriptorCurrent(i,d->action.get());
}
inline bool Owner::canRetireNativeTriggerDescriptors()const noexcept {
    if(!nativeTriggerList3D0_)return true;const auto& list=*nativeTriggerList3D0_;
    if(list.sourceMode)return canRetireSourceTriggerNative();
    if(list.descriptorSlotsPublished>2u||!list.actions)return false;
    if(list.descriptorsRetired)return !list.descriptors34&&!list.descriptorOwners[0]&&!list.descriptorOwners[1];
    if(!list.descriptors34&&list.descriptorSlotsPublished)return false;
    for(std::size_t i=0;i<2u;++i){
        const auto* d=list.descriptorOwners[i].get();
        if(i>=list.descriptorSlotsPublished){
            //Native custody of the allocation preceding an unavailable34 store.
            if(d&&(i!=list.descriptorSlotsPublished||list.descriptors34||d->authoring||
               d->trigger00||d->actionCount04||d->actions68||d->action))return false;
            continue;
        }
        if(list.descriptors34[i]!=d)return false;if(!d)continue;
        if(d->actionCount04<0||d->actionCount04>1||d->actionSlotsPublished>1||
           (!d->actions68&&d->actionSlotsPublished)||
           (d->actionSlotsPublished&&d->actions68[0]!=d->action.get())||
           (!d->actionSlotsPublished&&d->action&&d->actions68)||
           (d->complete&&!nativeTriggerDescriptorCurrent(i)))return false;
    }return true;
}
inline bool Owner::canRetireNativeTriggerList3D0()const noexcept {
    if(!nativeTriggerList3D0_)return true;
    if(nativeTriggerList3D0_->sourceMode)return canRetireSourceTriggerNative();
    if(!worldApplication_||worldApplication_->attachmentLock423F58())return false;
    const auto& list=*nativeTriggerList3D0_;
    if(!list.counted||!list.candidates||!list.authoring||!list.actions||list.joined>2u||list.count30>2u||
       list.actions!=list.candidates->triggerActions_||list.authoring!=list.candidates->triggerSource_||
       list.actions->lease_||list.servicesCompleted>2u||(list.serviceComplete&&list.servicesCompleted!=2u)||
       !list.actions->canRetireNativeLostAiCallbacks()||!canRetireNativeTriggerObservers()||
       !canRetireNativeTriggerDescriptors())return false;
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
        if(!nativeTriggerDescriptorCurrent(i)||
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

inline bool Owner::resetNativeParticles161F60(
    const std::shared_ptr<g::OriginalApplicationEntities50LA>& expected,
    std::uint64_t generation,std::uint32_t& pc)noexcept {
    pc=0x1ac96c;
    if(!expected||expected->nativeOwnerThread_!=std::this_thread::get_id())return false;
    auto& r=restartParticleReset_;
    if(r.attempted){pc=r.stoppedPC;return false;}
    r.attempted=true;r.stoppedPC=pc;
    if(worldApplication_!=expected||sceneApplication_!=expected||!generation||
       sceneGeneration_!=generation||!worldColdConstructed_||topologyClosed_||
       worldEntity_.sessionGeneration!=generation||worldEntity_.reference00!=1||
       expected->attachmentLock423F58()||!expected->entityAuthority_.canRetire(worldEntity_)||
       !nativeParticleManagerComplete_||!trackParticles_||
       nativeParticleManager4A0_!=trackParticles_.get())return false;
    auto& actual=*trackParticles_;
    // Counted real World4A0; do not preflight unrelated atlas/auxiliary fields
    // before161F60's active-pointer prefix, nor infer source-null from absence.
    if(actual.application_!=expected||actual.generation_!=generation||actual.closed_||
       !actual.entityConstructed_||!actual.particles_||
       actual.component_.kind!=g::OriginalWreckComponentKind50LA::particleManager160B10||
       actual.component_.reference00!=1||actual.component_.sessionGeneration!=generation||
       actual.component_.objectKey!=key(&actual.component_))return false;
    if(!actual.particles_->removeRestartParticles161F60(pc)){r.stoppedPC=pc;return false;}
    pc=r.stoppedPC=0x161fe8;
    // This successfully constructed actual162E70 owner is NOT a null branch.
    const auto* numeric=actual.particles_->auxiliaryChild();
    if(!actual.auxiliary1C0_||actual.auxiliary1C0_->reference00!=1||
       !numeric||!numeric->constructed||actual.auxiliary1C0_->numericChild!=numeric)return false;
    pc=r.stoppedPC=0x1634f0;r.auxiliaryGate18=numeric->enabled18;
    if(*r.auxiliaryGate18){
        //1634FC..50C scratch setup precedes signed count80 at163540.
        //That count/arrays are not represented by dormant cold resources.
        r.auxiliaryIndexA2=0;r.auxiliaryOffsetA3=0;r.auxiliaryFillA1=0x47c35000u;
        pc=r.stoppedPC=0x163540;return false;
    }
    pc=r.stoppedPC=0x162000;r.returned=true;return true;
}

inline bool Owner::retireNativeTriggerList3D0()noexcept {
    if(!nativeTriggerList3D0_)return true;
    if(nativeTriggerList3D0_->sourceMode)return false;
    // Actual Storage alone retires registration/membership. Source-owner
    // raw borrows were withdrawn before topology/storage destruction.
    if(runtimeQueries_||!canRetireNativeTriggerList3D0())return false;
    auto& list=*nativeTriggerList3D0_;auto& authority=worldApplication_->entityAuthority_;
    auto& count=worldApplication_->objectCount423F60();
    //2DC360 destroys descriptors FORWARD before1413E0 retires the Trigger
    //tree. Revoke action dispatch first; callback-array cleanup later uses only
    //retained pointer identity, never a dereference of retired action data.
    for(std::size_t i=0;i<2u;++i){
        auto& d=list.descriptorOwners[i];if(!d)continue;
        if(!list.actions->withdrawNativeActionDescriptor(i))std::terminate();
        //Geometry64 was native-adopted/consumed, not a guest allocation.
        d->action.reset();
        if(d->actionSlotsPublished)d->actions68[0]=nullptr;
        d->actions68.reset();d.reset();
    }
    list.descriptors34.reset();list.descriptorsRetired=true; //2DC3E0/E8.
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
        //Descriptor lifetime already ended before this Trigger tree release.
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
    if(nativeSunFlares_&&nativeSunFlares_->sourceMode)return false;
    pc=0x271e5c;
    if(!worldColdConstructed_||!worldApplication_||!worldAudio_||
       worldAudio_->coldThread_!=std::this_thread::get_id()||
       worldApplication_->attachmentLock423F58()||!worldEntity_.sessionGeneration)return false;
    if(!nativeSunFlarePointers4A4_)return false; //Actual271E5C read, unknown is not null.
    if(!nativeSunFlares_&&*nativeSunFlarePointers4A4_){pc=0x271e84;return false;}
    if(!nativeSunFlares_){
        //Explicit native join: this newly implemented private domain has never
        //owned a flare. No source270550 reset, existing-state clear or adoption
        //of unrelated sunlight/fire/garage owners is claimed or performed.
        nativeSunFlares_.emplace();nativeSunFlares_->generation=worldEntity_.sessionGeneration;
    }
    auto& d=*nativeSunFlares_;
    if(d.generation!=worldEntity_.sessionGeneration||!nativeSunFlarePointers4A4_)return false;
    if(d.count4A8&&*d.count4A8!=1u)return false;
    if(*nativeSunFlarePointers4A4_){
        //Retained SAME invocation only, not source append/new flare batch.
        return d.pointerBacking&&*nativeSunFlarePointers4A4_==d.pointerBacking.get()&&
            d.count4A8&&*d.count4A8==1u;
    }
    if(d.pointerBacking||d.owner)return false;
    if(d.pointerAllocationAttempted){pc=0x271e80;return false;}
    pc=0x271e68;d.count4A8=1u; //Source store before allocation; retained on failure.
    pc=0x271e74;d.pointerAllocationAttempted=true;
    try {d.pointerBacking=std::make_unique<NativeSunFlare*[]>(1);}catch(...){d.pointerBacking.reset();}
    pc=0x271e80;nativeSunFlarePointers4A4_=d.pointerBacking.get();
    if(!d.pointerBacking)return false; //Retain count and source-null allocation result publication.
    //Native null entry marks unconstructed ownership; source array element
    //is not claimed initialized until27209C publishes its real object.
    return true;
}
inline bool Owner::constructNativeSunFlare2C5730(NativeSunFlareInputs inputs,std::uint32_t& pc)noexcept {
    if(nativeSunFlares_&&nativeSunFlares_->sourceMode)return false;
    pc=0x27206c;
    if(!nativeSunFlares_||!worldApplication_||worldApplication_->attachmentLock423F58())return false;
    auto& d=*nativeSunFlares_;
    if(d.generation!=worldEntity_.sessionGeneration||!d.count4A8||*d.count4A8!=1||
       !d.pointerBacking||!nativeSunFlarePointers4A4_||
       *nativeSunFlarePointers4A4_!=d.pointerBacking.get())return false;
    if(d.owner){
        const auto& old=d.owner->inputs;
        return canRetireNativeSunFlare()&&old.texture==inputs.texture&&old.retrieveMaterial==inputs.retrieveMaterial&&
            old.modeA1==inputs.modeA1&&old.saturation==inputs.saturation&&old.minScale==inputs.minScale&&
            old.maxScale==inputs.maxScale&&old.width==inputs.width&&old.height==inputs.height&&
            old.vertexColor==inputs.vertexColor&&old.direction==inputs.direction;
    }
    if((*nativeSunFlarePointers4A4_)[0])return false;
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
    pc=0x27209c;(*nativeSunFlarePointers4A4_)[0]=d.owner.get(); //Publication BEFORE2C5A10.
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
    if(nativeSunFlares_&&nativeSunFlares_->sourceMode)return canRetireSourceSunFlareNative();
    if(!nativeSunFlares_)return true;
    const auto& d=*nativeSunFlares_;
    if(!nativeSunFlarePointers4A4_||*nativeSunFlarePointers4A4_!=d.pointerBacking.get())return false;
    if(!worldApplication_||worldApplication_->attachmentLock423F58()||
       d.generation!=worldEntity_.sessionGeneration||!d.count4A8||*d.count4A8!=1)return false;
    if(!d.owner)return !*nativeSunFlarePointers4A4_||!(*nativeSunFlarePointers4A4_)[0];
    const auto& o=*d.owner;const auto& e=o.entity;
    if((o.sprite64&&!canRetireNativeSunFlareMaterial(o.sprite64->material))||
       !canRetireNativeSunFlareMaterial(o.material68)||(o.callback74&&o.callback74!=&o))return false;
    if(o.sprite64&&o.sprite64->material.primitive04&&o.sprite64->material.primitive04->optional04)return false;
    if(o.material68.primitive04&&o.material68.primitive04->optional04&&
       (o.material68.primitive04->optional04->self!=&o||o.callback74!=&o))return false;
    if((o.colors44||o.colors48||o.buffer78)&&o.zones30!=10)return false;
    if(o.colorRowsInitialized>10||o.colorsInitialized!=(o.colorRowsInitialized==10)||
       (o.colorRowsInitialized&&(!o.colors44||!o.colors48||!o.depth40||!(o.flags98KnownValue&2u))))return false;
    return *nativeSunFlarePointers4A4_&&(*nativeSunFlarePointers4A4_)[0]==&o&&o.counted&&
        e.reference00==1&&e.kind==g::OriginalWreckComponentKind50LA::entity141300&&
        e.objectKey==key(&e)&&e.sessionGeneration==d.generation&&e.category28==0xff&&
        (e.flags29&15u)==3u&&!e.nativeFlags29&&!e.parent1C&&!e.firstChild18&&!e.previous10&&!e.next14&&
        !e.mask20&&!e.descendants24&&worldApplication_->objectCount423F60()&&
        worldApplication_->entityAuthority_.canRetire(e);
}
inline bool Owner::retireNativeSunFlare()noexcept {
    if(nativeSunFlares_&&nativeSunFlares_->sourceMode)return retireSourceSunFlareNative();
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
        (*nativeSunFlarePointers4A4_)[0]=nullptr;d.owner.reset();
    }
    nativeSunFlarePointers4A4_=nullptr;d.pointerBacking.reset();nativeSunFlares_.reset();return true;
}
}

namespace ra_pod_scene50LA {
inline const native_world_water50LA::Domain* Owner::readNativeAbsentWater400(std::uint32_t& pc)const noexcept {
    pc=0x1aa400;
    if(!worldWater400_||*worldWater400_||!nativeWaterDomain_||!worldColdConstructed_||!worldApplication_||
       worldApplication_->attachmentLock423F58()||worldEntity_.objectKey!=key(&worldEntity_)||
       worldEntity_.reference00!=1u||!worldEntity_.sessionGeneration||
       !nativeWaterDomain_->readAbsent(*worldApplication_,worldEntity_,worldEntity_.sessionGeneration))return nullptr;
    //Actual source400 is known null from2705D0 and unchanged by both
    //authenticated none branches. Typed capability remains the same owned
    //native domain; it cannot establish source-null independently anymore.
    return nativeWaterDomain_.get();
}
}
namespace ra_cold_aggregate50LA {
inline bool Owner::nativeWaterDomainMatches(const native_world_water50LA::Domain& domain)const noexcept {
    return !closed_&&!terminal_&&worldColdConstructed_&&worldWater400_&&!*worldWater400_&&worldApplication_&&source_&&
        source_->sourcesDecoded()&&nativeWaterDomain_.get()==&domain&&
        !worldApplication_->attachmentLock423F58()&&worldEntity_.reference00==1u&&
        worldEntity_.objectKey==key(&worldEntity_)&&worldEntity_.sessionGeneration&&
        domain.matches(*worldApplication_,worldEntity_,worldEntity_.sessionGeneration,
            source_->sourcesDecoded()->height);
}
}

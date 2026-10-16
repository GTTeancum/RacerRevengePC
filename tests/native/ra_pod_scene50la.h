#pragma once
#include "ra_source_camera_ownership50la.h"
#include "original_world_physics_index50la.h"
#include "original_world_collision_objects50la.h"
#include "ra_collision_resources50la.h"
#include "../../src/game/original_track_particle_cold50la.h"
#include "../../src/game/original_visibility_quadtree50la.h"
#include <array>
#include <limits>
#include <memory>
#include "../../platform/linux/native/race_audio_groups50ic.h"

namespace rr::game {class OriginalRaceConstructor50LA;}
namespace ra_cold_aggregate50LA {class Owner;}
namespace ra_pod_scene50LA {
namespace g=rr::game;
// The scene owns the EXISTING canonical visibility, collision and particle
// owners. The derived cold aggregate supplies their source-bound construction
// steps; these are not copies, replacement registries or alternate query worlds.
class Owner {
    friend class ::ra_cold_aggregate50LA::Owner;
    friend class rr::game::OriginalRaceConstructor50LA;
protected:
    using Entity=g::OriginalWreckComponent50LA;
    std::shared_ptr<g::original_gc_gameplay_lighting_candidate::Owner> lighting_;
    std::shared_ptr<ra_source_camera50LA::OwnershipBridge> runtimeQueries_;
    std::shared_ptr<g::OriginalTrackParticleCold50LA> trackParticles_;
private:
    // Constructor-only prefix on this SAME concrete scene. No second world.
    std::unique_ptr<g::OriginalWorldPhysicsIndex50LA> worldPhysics3D4_;
    bool prepareWorldPhysicsIndex270CDC(std::shared_ptr<const ra_cold_sources50LA::Owner>,std::uint32_t&)noexcept;
    std::unique_ptr<g::OriginalWorldCollisionObjects50LA> worldCollisionObjects3CC_;
    bool prepareWorldCollisionObjects271230(std::shared_ptr<const ra_cold_sources50LA::Owner>,std::uint32_t&)noexcept;
    bool canReleaseWorldCollisionObjects27BB80(std::uint32_t&)const noexcept;
    bool releaseWorldCollisionObjects27BB80(std::uint32_t&)noexcept;
    std::unique_ptr<ra_collision_resources50LA::Owner> worldCollisionResources_;
    bool prepareWorldCollisionResources(std::shared_ptr<ra_static_candidates50LA::Owner>,
        std::shared_ptr<rr::assets::OriginalStandardTextureResolver50LA>,rr::assets::OriginalStandardTextureDiagnostic50LA&)noexcept;
    Entity worldEntity_;
    std::shared_ptr<g::OriginalApplicationEntities50LA> worldApplication_;
    // Native collision storage can precede the counted world prefix. Retain
    // its actual process identity so either acquisition order must agree.
    std::shared_ptr<g::OriginalApplicationEntities50LA> collisionApplication_;
    rr::platform::RaceAudioGroups50IC* worldAudio_=nullptr; //Caller lifetime through rollback/retirement.
    std::unique_ptr<Entity*[]> worldList500_;
    std::uint32_t worldListCount50C_=0,worldListCapacity504_=0,worldListGrowth508_=0;
    std::uint32_t worldMode30_=0,worldMode524_=0;
    bool worldColdConstructed_=false,worldColdAttempted_=false;
    bool constructWorldColdPrefix(std::shared_ptr<g::OriginalApplicationEntities50LA>,
        rr::platform::RaceAudioGroups50IC&,rr::platform::AudioMixer50HD&,std::uint64_t,std::uint32_t&)noexcept;
    bool rollbackNativeWorldConstructionFailure(std::uint32_t&)noexcept;
    bool canRetireNativeWorldPrefix(const char** gate=nullptr)const noexcept;
    bool retireNativeWorldPrefixAfterOwnedCleanup(std::uint32_t&)noexcept;
    struct SearchNode {
        Entity entity;
        // Exact stable canonical row. This borrow does NOT claim the source
        //27F420/27F490 reference acquisitions have run for its methods.
        const g::CollectionZone50BX* collectionBacking=nullptr;
        std::uint32_t ordinal48=0;
        bool constructed=false,attached=false;
    };
    // RA's authenticated ten zones; capacity is not a caller-provided count.
    std::array<SearchNode,10> visibility4C0_,collision4C4_;
    Entity container4BC_;
    SearchNode environment443108_;
    std::unique_ptr<g::OriginalVisibilityQuadTree50LA> environmentMethod_;
    bool method34Retained_=false,method38Retained_=false,constructorReferenceReleased_=false;
    std::array<Entity*,22> domain_{};
    std::size_t domainCount_=0;
    std::shared_ptr<g::OriginalApplicationEntities50LA> sceneApplication_;
    std::uint64_t sceneGeneration_=0;
    const g::CollectionZone50BX* visibilityRows_=nullptr;
    const g::CollectionZone50BX* collisionRows_=nullptr;
    bool topologyAttempted_=false,topologyConstructed_=false,topologyClosed_=false;
    bool particlePredecessorAttempted_=false;
    std::uint32_t useEnvironment35C_=0; //270578, then authenticated270A1C default0.
    static std::uint64_t key(const void* p)noexcept {
        return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(p));
    }
    bool fail(std::uint32_t& pc,std::uint32_t source)const noexcept{pc=source;return false;}
    bool identitiesCurrent()const noexcept {
        if(!lighting_||!runtimeQueries_||!sceneApplication_||!sceneApplication_->application10()||
           runtimeQueries_->sceneOwner_!=this||!runtimeQueries_->storage_.get())return false;
        const auto& s=*runtimeQueries_->storage_.get();
        return s.zones.data()==collisionRows_&&s.zones.size()==10u&&
            s.lifecycle.storage.views.zones==collisionRows_;
    }
    bool constructTopology(std::shared_ptr<g::OriginalApplicationEntities50LA> application,
        std::uint64_t generation,std::uint32_t& pc)noexcept {
        pc=0;
        if(topologyAttempted_||topologyClosed_)return fail(pc,0x2729e0);
        if(worldColdConstructed_&&(application!=worldApplication_||generation!=worldEntity_.sessionGeneration))
            return fail(pc,0x2729e0);
        if(!application||!generation||!application->application10()||
           application->attachmentLock423F58()||!lighting_||!runtimeQueries_||
           !runtimeQueries_->candidates_||!runtimeQueries_->candidates_->decodedSource()||
           !runtimeQueries_->storageConstructed_||runtimeQueries_->closed_||runtimeQueries_->terminal_||
           runtimeQueries_->sceneOwner_||!runtimeQueries_->storage_.get()||!lighting_->world())
            return fail(pc,0x2729e0);
        const auto& source=runtimeQueries_->candidates_->decodedSource()->world;
        const auto& visible=*lighting_->world();
        const auto& collision=*runtimeQueries_->storage_.get();
        if(source.parameters.original_bytes!=visible.parameters.original_bytes||source.zones.size()!=10u||
           collision.zones.size()!=10u||collision.lifecycle.zoneCount!=10u||
           collision.lifecycle.storage.views.zones!=collision.zones.data())return fail(pc,0x272a48);
        // The genuine RA fresh reader defaults UseEnvironmentNode to zero.
        // Unknown explicit authoring is rejected; it is never forced false.
        for(const auto& b:source.parameters.blocks)if(b.name=="sceneinfo")
            for(const auto& f:b.fields)if(f.name=="useenvironmentnode")return fail(pc,0x270a1c);
        if(application->objectCount423F60()>std::numeric_limits<std::uint32_t>::max()-22u)
            return fail(pc,0x1413b8);
        topologyAttempted_=true;sceneApplication_=std::move(application);sceneGeneration_=generation;
        std::size_t count=0;
        visibilityRows_=lighting_->retainSceneZones272A78(this,count);
        if(!visibilityRows_||count!=10u)return fail(pc,0x272a78);
        collisionRows_=collision.zones.data();runtimeQueries_->sceneOwner_=this;
        auto construct=[&](Entity& entity,g::OriginalWreckComponentKind50LA kind)->bool {
            const auto result=g::initializeOriginalWreckComponent50LA(entity,kind,generation,key(&entity),true,
                sceneApplication_->objectCount423F60());
            if(result.status!=g::OriginalWreckComponentStatus50LA::complete)return fail(pc,result.sourcePC);
            domain_[domainCount_++]=&entity;return true;
        };
        if(!construct(container4BC_,g::OriginalWreckComponentKind50LA::entity141300))return false;
        //272A58..272AE4: visibility constructor, collision constructor, then
        //both ordinal stores, in authored order. No existing row is rewritten.
        for(std::size_t z=0;z<10u;++z){
            auto& v=visibility4C0_[z];auto& c=collision4C4_[z];
            if(!construct(v.entity,g::OriginalWreckComponentKind50LA::searchNode27F120))return false;
            v.constructed=true;v.collectionBacking=&visibilityRows_[z];
            if(!construct(c.entity,g::OriginalWreckComponentKind50LA::searchNode27F120))return false;
            c.constructed=true;c.collectionBacking=&collisionRows_[z];
            v.ordinal48=c.ordinal48=static_cast<std::uint32_t>(z);
        }
        const g::OriginalWreckComponentDomain50LA domain{generation,domain_.data(),domainCount_};
        for(std::size_t z=0;z<10u;++z){
            auto& v=visibility4C0_[z];auto& c=collision4C4_[z];
            auto result=g::appendOriginalWreckComponent50LA(domain,container4BC_,v.entity,0x400u,
                sceneApplication_->attachmentLock423F58());
            if(result.status!=g::OriginalWreckComponentStatus50LA::complete)return fail(pc,result.sourcePC);
            v.attached=true;
            result=g::appendOriginalWreckComponent50LA(domain,container4BC_,c.entity,0u,
                sceneApplication_->attachmentLock423F58());
            if(result.status!=g::OriginalWreckComponentStatus50LA::complete)return fail(pc,result.sourcePC);
            c.attached=true;
        }
        topologyConstructed_=true;return true;
    }
    bool particlePredecessor1B4DC0(std::uint32_t& pc)noexcept {
        pc=0;
        if(particlePredecessorAttempted_||!topologyConstructed_||topologyClosed_||!identitiesCurrent())
            return fail(pc,0x1b4dc0);
        particlePredecessorAttempted_=true;
        //The caller checked the actual same-SCB particle selector. Read the
        //one Application slot, never a local stand-in for a surviving method.
        if(sceneApplication_->visibility443A98_||sceneApplication_->environment443108_)
            return fail(pc,0x1b4e10); //Existing-scene adoption needs its real owner.
        const auto* bounds=lighting_->sceneBounds534(this);
        if(!bounds)return fail(pc,0x273004);
        useEnvironment35C_=1; //1B4E20, before the actual272FC0 constructor.
        try {environmentMethod_.reset(new g::OriginalVisibilityQuadTree50LA);}
        catch(...){return fail(pc,0x272fe4);}
        if(!environmentMethod_->construct(*sceneApplication_,*bounds,pc))return false;
        auto& e=environment443108_;
        auto result=g::initializeOriginalWreckComponent50LA(e.entity,
            g::OriginalWreckComponentKind50LA::searchNode27F120,sceneGeneration_,key(&e.entity),true,
            sceneApplication_->objectCount423F60());
        if(result.status!=g::OriginalWreckComponentStatus50LA::complete)return fail(pc,result.sourcePC);
        e.constructed=true;e.ordinal48=0;domain_[domainCount_++]=&e.entity;
        sceneApplication_->environment443108_=&e.entity;
        const g::OriginalWreckComponentDomain50LA domain{sceneGeneration_,domain_.data(),domainCount_};
        result=g::appendOriginalWreckComponent50LA(domain,container4BC_,e.entity,0,
            sceneApplication_->attachmentLock423F58());
        if(result.status!=g::OriginalWreckComponentStatus50LA::complete)return fail(pc,result.sourcePC);
        e.attached=true;
        //27F420/27F490 both retain the SAME method; usage10's second store wins.
        auto& method=*environmentMethod_;
        method.usage10_=1;++method.reference00_;method34Retained_=true;
        method.usage10_=2;++method.reference00_;method38Retained_=true;
        --method.reference00_;constructorReferenceReleased_=true; //27315C.
        //A genuine newly-constructed tree has no registered objects yet. This
        //is NOT proof that any later camera may return an empty success.
        pc=0;return true;
    }
    bool canCloseTopology()const noexcept {
        if(topologyClosed_||!topologyAttempted_)return true;
        if(!sceneApplication_||sceneApplication_->attachmentLock423F58())return false;
        if(visibilityRows_&&(!lighting_||!lighting_->canReleaseSceneZones272A78(this)))return false;
        if(collisionRows_&&(!runtimeQueries_||runtimeQueries_->sceneOwner_!=this))return false;
        std::size_t counted=0;
        for(std::size_t i=0;i<domainCount_;++i){const auto* e=domain_[i];
            if(!e||e->reference00!=1u||e->sessionGeneration!=sceneGeneration_||e->objectKey!=key(e))return false;
            if(e!=&container4BC_&&e->firstChild18)return false;
            ++counted;
        }
        if(counted>sceneApplication_->objectCount423F60()||container4BC_.parent1C||
           container4BC_.previous10||container4BC_.next14)return false;
        const Entity* previous=nullptr;const Entity* child=container4BC_.firstChild18;
        for(std::size_t z=0;z<10u;++z)for(const auto* node:{&visibility4C0_[z],&collision4C4_[z]}){
            if(node->attached){
                if(child!=&node->entity||child->parent1C!=&container4BC_||child->previous10!=previous||
                   child->mask20!=(node==&visibility4C0_[z]?0x400u:0u))return false;
                previous=child;child=child->next14;
            }else if(node->entity.parent1C||node->entity.previous10||node->entity.next14)return false;
        }
        const auto& environment=environment443108_;
        if(environment.attached){
            if(child!=&environment.entity||child->parent1C!=&container4BC_||
               child->previous10!=previous||child->mask20||child->next14)return false;
            child=child->next14;
        }else if(environment.entity.parent1C||environment.entity.previous10||environment.entity.next14)return false;
        if(environment.constructed&&sceneApplication_->environment443108_!=&environment.entity)return false;
        if(environmentMethod_){
            const auto& method=*environmentMethod_;
            const std::uint64_t expected=(!method.application_||constructorReferenceReleased_?0u:1u)+
                std::uint64_t(method34Retained_)+std::uint64_t(method38Retained_);
            if(method.reference00_!=expected||!method.constructorStateOnly()||
               (method.published_&&sceneApplication_->visibility443A98_!=&method))return false;
        }
        return !child;
    }
    bool closeTopology()noexcept {
        if(topologyClosed_)return true;
        if(!canCloseTopology())return false;
        if(visibilityRows_&&!lighting_->releaseSceneZones272A78(this))return false;
        //Canonical zone methods remain with their original owners. The
        //fallback node releases its real34 then38 references (27F1EC/27F208).
        //142590 ->1424E0 visits linked children tail-to-head, then4BC.
        Entity* tail=container4BC_.firstChild18;
        while(tail&&tail->next14)tail=tail->next14;
        while(tail){auto* child=tail;tail=child->previous10;
            if(tail)tail->next14=nullptr;else container4BC_.firstChild18=nullptr;
            child->parent1C=nullptr;child->previous10=nullptr;child->next14=nullptr;
            --child->reference00; //104970 reaches the concrete destructor at zero.
            if(child==&environment443108_.entity){
                auto& method=*environmentMethod_;
                if(method34Retained_){--method.reference00_;method34Retained_=false;}
                //Last release runs the concrete destructor while its ref is1.
                if(method38Retained_){
                    if(!method.closeConstructor())return false;
                    method38Retained_=false;
                }
                sceneApplication_->environment443108_=nullptr;
            }
            --sceneApplication_->objectCount423F60(); //Concrete destructor ->1413E0.
        }
        for(std::size_t i=1;i<domainCount_;++i)if(domain_[i]->reference00){
            domain_[i]->reference00=0;--sceneApplication_->objectCount423F60();
        }
        if(environment443108_.constructed)sceneApplication_->environment443108_=nullptr;
        if(environmentMethod_&&environmentMethod_->reference00_&&!environmentMethod_->closeConstructor())return false;
        environmentMethod_.reset();
        if(container4BC_.reference00){container4BC_.reference00=0;--sceneApplication_->objectCount423F60();}
        container4BC_.descendants24=0;
        if(runtimeQueries_&&runtimeQueries_->sceneOwner_==this)runtimeQueries_->sceneOwner_=nullptr;
        visibilityRows_=collisionRows_=nullptr;sceneApplication_.reset();topologyClosed_=true;return true;
    }
protected:
    Owner()=default;
    ~Owner(){if(worldColdConstructed_||(topologyAttempted_&&!topologyClosed_))std::terminate();}
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
};
}

// Included outside namespaces after actual World/Bridge owner definitions.
#include "../../src/game/collection_bounds48s.h"
#include "../../src/game/portal_geometry50eo.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>
#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__)
#error "RA trigger observer requires strict binary32"
#endif
static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559&&
              std::numeric_limits<float>::digits==24,"RA trigger observer binary32");
namespace ra_pod_scene50LA {
bool Owner::nativeTriggerObserverCurrent(std::size_t i)const noexcept {
    if(!nativeTriggerList3D0_||nativeTriggerList3D0_->sourceMode||i>=2u)return false;
    const auto& list=*nativeTriggerList3D0_;const auto* o=list.observers[i].get();
    if(!o||o->retiring||!runtimeQueries_||runtimeQueries_->sceneOwner_!=this||
       !sceneApplication_||sceneApplication_!=worldApplication_||!list.actions||
       list.actions->lease_||!list.candidates||runtimeQueries_->candidates_!=list.candidates||
       !nativeCollisionListsCurrent()||!runtimeQueries_->triggerMembership_[i]||
       list.candidates->candidates_.size()<180u||list.actions->group_.triggers.size()<2u)return false;
    const auto* s=runtimeQueries_->storage_.get();const auto* m=runtimeQueries_->triggerMembership_[i].get();
    if(!s||o->storageBorrow!=s||o->membershipBorrow!=m||!s->borrowedWorldLease||
       !m->state.attached||m->state.object!=179u+i||m->state.method!=g::ZoneMethod50CL::First||
       m->state.revision!=o->membershipRevision||m->state.storage.links!=m->links.data()||
       m->state.storage.entries!=m->entries.data()||m->state.storage.capacity!=m->links.size()||
       m->links.size()!=m->entries.size()||!m->links.size()||
       s->registries.size()!=20u||s->lifecycle.storage.registries!=s->registries.data()||
       s->lifecycle.storage.registrations!=s->registrations.data()||s->registrations.size()<=178u+i||
       s->candidates.size()<=178u+i||s->candidates[178u+i]!=&list.candidates->candidates_[178u+i])return false;
    const auto& pair=list.actions->nativeEntities_[i];
    const auto& candidate=list.candidates->candidates_[178u+i];
    if(!pair.triggerCounted||!pair.colliderCounted||!pair.attached||
       pair.collider.parent1C!=&pair.trigger||pair.collider.mask20!=2u||
       pair.collider.reference00!=1u||pair.collider.sessionGeneration!=sceneGeneration_||
       pair.collider.nativeFlags29!=&candidate.candidate.collision.filter.flags31||
       !sceneApplication_->entityAuthority_.canRetire(pair.collider)||
       candidate.nativeTriggers!=&list.actions->group_||candidate.nativeTriggerIndex!=i||
       candidate.shapeType!=5u||candidate.candidate.collision.shapeType!=5u)return false;
    const auto validEntry=[&](std::size_t n){
        if(n>=m->entries.size()||!m->entries[n].used)return false;
        const auto zone=m->links[n].zone;if(!zone||zone>10u)return false;
        const auto& r=m->entries[n].registration;const auto& reg=s->registries[(zone-1u)*2u];
        const auto* method=collision4C4_[zone-1u].nativeList34;
        return method&&method->world_==this&&method->application_==sceneApplication_.get()&&
            method->registry_==&reg&&method->method_==&s->trees[(zone-1u)*2u]&&
            method->ordinal_==(zone-1u)*2u&&method->reference00_&&method->nodeReference_&&
            r.owner==&reg&&r.encodedObject==(179u+i)*4u&&r.slot&&
            reg.blockCount==1u&&reg.perBlock&&reg.perBlock<=1024u&&
            reg.lastUsed<=reg.perBlock&&r.slot<=reg.lastUsed&&r.slot<=reg.perBlock&&
            reg.words[r.slot-1u]==r.encodedObject;
    };
    if(!validEntry(0)||m->links[0].zone!=4u||m->entries[0].portal||
       s->registrations[178u+i].owner!=m->entries[0].registration.owner||
       s->registrations[178u+i].slot!=m->entries[0].registration.slot||
       s->registrations[178u+i].encodedObject!=m->entries[0].registration.encodedObject)return false;
    const auto& pool=sceneApplication_->garageObserverShared_;
    if(o->temporaryConstructed&&!pool.owns(&o->temporary))return false;
    if(o->persistentConstructed&&!pool.owns(&o->persistent))return false;
    if(o->complete&&(!o->persistentConstructed||o->temporaryConstructed||
       o->persistent.child04!=&collision4C4_[3].entity||
       o->persistent.method08!=collision4C4_[3].nativeList34||
       o->persistent.slot14!=m->entries[0].registration.slot))return false;
    std::size_t visited=0;
    for(auto ref=m->links[0].next;ref;ref=m->links[ref-1u].next){
        if(++visited>m->entries.size()||ref==1u||ref>m->entries.size()||!validEntry(ref-1u))return false;
    }
    std::size_t used=0;for(std::size_t n=1;n<m->entries.size();++n)if(m->entries[n].used)++used;
    if(used!=visited)return false;
    for(std::size_t n=0;n<o->links.size();++n){const auto& owned=o->links[n];
        if(!owned.link||!o->persistentConstructed||!pool.ownsLink(&o->persistent,owned.link)||
           !owned.membershipIndex||!validEntry(owned.membershipIndex))return false;
        for(std::size_t j=0;j<n;++j)if(o->links[j].link==owned.link||
            o->links[j].membershipIndex==owned.membershipIndex)return false;
        const auto& e=m->entries[owned.membershipIndex];const auto z=m->links[owned.membershipIndex].zone;
        if(!e.portal||e.portal>nativePortals_.size()||!nativePortals_[e.portal-1u]||
           owned.link->method08||owned.link->collisionMethod08!=collision4C4_[z-1u].nativeList34||
           owned.link->child04!=&collision4C4_[z-1u].entity||
           owned.link->portal0C!=&nativePortals_[e.portal-1u]->entity||
           owned.link->slot14!=e.registration.slot)return false;
        std::uint32_t bits=0;std::memcpy(&bits,&e.distance10,4);
        if(owned.link->distance10!=bits)return false;
    }
    if(o->complete){
        if(o->links.size()!=visited)return false;
        auto* link=o->persistent.next00;auto ref=m->links[0].next;
        for(std::size_t n=0;n<visited;++n){
            const NativeTriggerObserver::Secondary* found=nullptr;
            for(const auto& owned:o->links)if(owned.link==link)found=&owned;
            if(!found||!ref||found->membershipIndex!=ref-1u)return false;
            link=link->next00;ref=m->links[ref-1u].next;
        }
        if(link||ref)return false;
    }
    return true;
}

bool Owner::configureNativeTriggerObserver2A0A80(std::size_t i,std::uint32_t& pc)noexcept {
    pc=0x2a0b00;
    if(!nativeTriggerList3D0_||nativeTriggerList3D0_->sourceMode||i>=2u||!runtimeQueries_||!sceneApplication_||
       runtimeQueries_->sceneOwner_!=this||runtimeQueries_->closed_||runtimeQueries_->terminal_||
       runtimeQueries_->nativeFifoPins_||runtimeQueries_->triggerQueryActive_||
       runtimeQueries_->sharedOwnerLease_||runtimeQueries_->sharedRoleViews_||
       (runtimeQueries_->endCandidateProducer_&&runtimeQueries_->endCandidateProducer_->inCall)||
       sceneApplication_->attachmentLock423F58()||std::fegetround()!=FE_TONEAREST)return false;
    auto& list=*nativeTriggerList3D0_;
    if(list.observers[i]){if(list.observers[i]->complete){pc=0x2a0b1c;return nativeTriggerObserverCurrent(i);}return false;}
    if(!list.authoring||!list.authoring->sources()||!list.candidates||!list.actions||
       !runtimeQueries_->triggerMembership_[i])return false;
    auto* s=runtimeQueries_->storage_.get();auto& m=*runtimeQueries_->triggerMembership_[i];
    try{list.observers[i]=std::make_unique<NativeTriggerObserver>();}catch(...){return false;}
    auto& o=*list.observers[i];o.attempted=true;o.storageBorrow=s;o.membershipBorrow=&m;
    o.membershipRevision=m.state.revision;
    const auto reject=[&](std::uint32_t at){pc=at;o.failurePC=at;
        // Before acquiring any source resource there is no runtime borrow to
        // preserve. Invalid-admission/allocation failures still retire safely.
        if(!o.temporaryConstructed&&!o.persistentConstructed&&o.links.empty()){
            o.storageBorrow=nullptr;o.membershipBorrow=nullptr;o.retiring=true;
        }
        return false;};
    if(!nativeTriggerObserverCurrent(i)||!s||m.state.revision!=2u)return reject(pc);
    const auto& authored=(*list.authoring->sources())[i];
    const auto& candidate=list.candidates->candidates_[178u+i];const auto& c=candidate.candidate.collision;
    if(authored.zone!=4u||!std::isfinite(authored.sphere.radius)||authored.sphere.radius<=0.f||
       c.sphere.radius!=authored.sphere.radius||m.state.bounds.cachedRadius!=authored.sphere.radius||
       m.state.lastShapeTag!=5u||c.filter.categoryA4!=8u||
       (c.filter.flagsF8&0x3fu)!=6u||c.filter.identity!=ra_trigger_actions50LA::firstTriggerIdentity+i)return reject(pc);
    // 11F680(shape5) would copy actual trigger50 matrix into shape10 and
    // transform the source sphere center. The SAME native candidate already
    // owns that immutable identity placement. Adopt it, never replay geometry,
    // cached center, previous pose or existing live membership initialization.
    for(std::size_t n=0;n<16u;++n)if(c.geometry.worldMatrix.v[n]!=(n%5u==0u?1.f:0.f))return reject(pc);
    for(std::size_t n=0;n<3u;++n)if(c.sphere.center.v[n]!=authored.sphere.center.v[n]||
       m.state.bounds.center.v[n]!=authored.sphere.center.v[n])return reject(pc);
    o.placementAdopted=true;
    auto& pool=sceneApplication_->garageObserverShared_;
    pc=0x120094;if(!pool.construct280890(&o.temporary))return reject(pc);o.temporaryConstructed=true;
    // Actual flagsF8 bit3 clear chooses persistent E0. Existing native E0 is
    // adopted, NOT destroyed/re-attached at1200C4; this adds missing source
    // allocator custody without replaying the already-live registration.
    pc=0x1200e0;if(!pool.construct280890(&o.persistent))return reject(pc);o.persistentConstructed=true;
    pc=0x1202e0;g::CollectionBoundsResult48S bounds{};
    const auto input=ra_trigger_candidates50LA::bounds(authored);
    if(g::prepareCollectionBounds48S(input.bounds,bounds)!=g::CollectionBoundsStatus48S::Complete)return reject(pc);
    for(std::size_t n=0;n<3u;++n)if(bounds.center.v[n]!=m.state.bounds.center.v[n])return reject(pc);
    o.persistent.child04=&collision4C4_[3].entity;o.persistent.method08=collision4C4_[3].nativeList34;
    o.persistent.slot14=m.entries[0].registration.slot;
    for(std::size_t n=0;n<4u;++n)o.persistent.center20[n]=bounds.center.v[n];
    const auto* decoded=list.candidates->source_->sourcesDecoded();
    if(!decoded||!s->borrowedWorldLease||s->world.zoneCount!=10u||s->world.portalCount!=10u||
       s->world.portalLinkCount!=20u||s->world.portalHeads!=decoded->liveWorld.portalHeads||
       s->world.portalLinks!=decoded->liveWorld.portalLinks||s->world.portals!=decoded->liveWorld.portals)return reject(0x280bf0);
    // Reserve native custody before105720; every acquired link remains owned
    // even when subsequent validation fails. No source34 zero-value claim.
    try{o.links.reserve(m.entries.size());}catch(...){return reject(0x105720);}
    auto ref=s->world.portalHeads[3];std::size_t traversed=0;
    for(auto* edge=collision4C4_[3].portalLinks30.get();edge;edge=edge->next00.get()){
        pc=0x280c40;if(++traversed>10u||!ref||ref>s->world.portalLinkCount)return reject(pc);
        const auto& live=s->world.portalLinks[ref-1u];
        if(!live.portal||live.portal>nativePortals_.size()||!live.zone||live.zone>10u)return reject(pc);
        const auto& portal=nativePortals_[live.portal-1u];
        if(!portal||!portal->loaded||!portal->attached||portal->worldLinksPublished!=4u||
           !portal->geometry||edge->portal08!=&portal->entity||
           edge->neighbor04!=&collision4C4_[live.zone-1u].entity)return reject(pc);
        const auto* actual=lighting_?lighting_->nativeRaPortal230450(this,decoded->world,
            visibilityRows_,sceneApplication_.get(),live.portal-1u,pc):nullptr;
        if(actual!=portal->geometry||!actual)return reject(pc);
        const auto& collision=s->world.portals[live.portal-1u];
        for(std::size_t a=0;a<3u;++a){if(actual->normal.v[a]!=collision.normal.v[a])return reject(pc);
            for(std::size_t v=0;v<4u;++v)if(actual->vertices[v].v[a]!=collision.vertices[v].v[a]||
                actual->edgeNormals[v].v[a]!=collision.edgeNormals[v].v[a])return reject(pc);}
        g::Vector3_48C center{{bounds.center.v[0],bounds.center.v[1],bounds.center.v[2]}};
        bool initialHit=false,laterHit=false;float initialDistance=0,laterDistance=0;pc=0x231150;
        if(g::overlapPortal231150_50EO(*actual,center,0.f,initialHit,initialDistance)!=g::PortalGeometryStatus50EO::Complete||
           g::overlapPortal231150_50EO(*actual,center,authored.sphere.radius,laterHit,laterDistance)!=g::PortalGeometryStatus50EO::Complete)return reject(pc);
        std::size_t found=m.entries.size();
        for(std::size_t n=1;n<m.entries.size();++n)if(m.entries[n].used&&m.entries[n].portal==live.portal){
            if(found!=m.entries.size()||m.links[n].zone!=live.zone)return reject(pc);found=n;}
        // Existing producer really performed attach(radius0), then update(radius).
        // Any retained later entry is admitted as native later-state adoption,
        // never mislabeled a source initial-radius hit or silently removed.
        if(initialHit&&!laterHit)return reject(pc);
        if(laterHit!=(found!=m.entries.size()))return reject(pc);
        if(found!=m.entries.size()){
            if(m.entries[found].distance10!=laterDistance)return reject(pc);
            pc=0x105720;auto* link=pool.allocate105720(&o.persistent);if(!link)return reject(pc);
            o.links.push_back({link,found,initialHit});
            link->next00=o.persistent.next00;link->child04=&collision4C4_[live.zone-1u].entity;
            link->method08=nullptr;link->collisionMethod08=collision4C4_[live.zone-1u].nativeList34;
            link->portal0C=&portal->entity;std::memcpy(&link->distance10,&m.entries[found].distance10,4);
            link->slot14=m.entries[found].registration.slot;o.persistent.next00=link;
        }
        ref=live.next;
    }
    if(ref)return reject(0x280c40);
    if(!nativeTriggerObserverCurrent(i))return reject(0x280b88);
    pc=0x120200;if(!pool.release280920(&o.temporary))return reject(pc);o.temporaryConstructed=false;
    o.complete=true;if(!nativeTriggerObserverCurrent(i)){o.complete=false;return reject(0x280c40);}
    pc=0x2a0b1c;return true;
}

bool Owner::canRetireNativeTriggerObservers()const noexcept {
    if(!nativeTriggerList3D0_)return true;
    if(nativeTriggerList3D0_->sourceMode)return canRetireSourceTriggerNative();
    if(!worldApplication_||worldApplication_->attachmentLock423F58()||!nativeTriggerList3D0_->actions||
       nativeTriggerList3D0_->actions->lease_)return false;
    if(runtimeQueries_&&(runtimeQueries_->nativeFifoPins_||runtimeQueries_->triggerQueryActive_||
       runtimeQueries_->sharedOwnerLease_||runtimeQueries_->sharedRoleViews_||
       (runtimeQueries_->endCandidateProducer_&&runtimeQueries_->endCandidateProducer_->inCall)))return false;
    const auto& pool=worldApplication_->garageObserverShared_;
    for(std::size_t i=0;i<2u;++i){const auto* o=nativeTriggerList3D0_->observers[i].get();if(!o)continue;
        if(!o->retiring&&!nativeTriggerObserverCurrent(i))return false;
        if(o->retiring&&(o->storageBorrow||o->membershipBorrow||o->persistent.child04||
           o->persistent.method08||o->temporary.child04||o->temporary.method08))return false;
        if(o->temporaryConstructed&&(!pool.owns(&o->temporary)||pool.ownerHasLinks(&o->temporary)))return false;
        if(o->persistentConstructed&&!pool.owns(&o->persistent))return false;
        if(!o->persistentConstructed&&!o->links.empty())return false;
        for(std::size_t n=0;n<o->links.size();++n){const auto* link=o->links[n].link;
            if(!pool.ownsLink(&o->persistent,link))return false;
            for(std::size_t j=0;j<n;++j)if(o->links[j].link==link)return false;
            if(o->retiring&&(link->child04||link->method08||link->collisionMethod08||link->portal0C))return false;
        }
    }
    return true;
}
bool Owner::withdrawNativeTriggerObserverStorage()noexcept {
    if(!canRetireNativeTriggerObservers())return false;
    if(!nativeTriggerList3D0_)return true;
    // Native teardown boundary BEFORE closeTopology and Storage destruction.
    // No unregister: actual Storage alone owns its primary/secondary records.
    // Clear every owner borrow before portal/node/method lifetime ends. Keep
    // genuine allocator ownership until the collider's later retirement.
    for(auto& owned:nativeTriggerList3D0_->observers)if(owned){auto& o=*owned;
        if(o.retiring)continue;
        o.storageBorrow=nullptr;o.membershipBorrow=nullptr;
        o.persistent.child04=nullptr;o.persistent.method08=nullptr;
        o.temporary.child04=nullptr;o.temporary.method08=nullptr;
        for(auto& entry:o.links){auto* link=entry.link;
            link->child04=nullptr;link->method08=nullptr;link->collisionMethod08=nullptr;link->portal0C=nullptr;}
        o.retiring=true;
    }
    return true;
}
bool Owner::retireNativeTriggerObserver2A0420(std::size_t i,std::uint32_t& pc)noexcept {
    pc=0x2a0420;if(!nativeTriggerList3D0_||nativeTriggerList3D0_->sourceMode||i>=2u)return false;
    auto& owned=nativeTriggerList3D0_->observers[i];if(!owned)return true;
    // No dereference of Storage/registries or zone methods after withdrawal.
    // Exact original281490 unregister is NOT claimed; native Storage already
    // owns it. These are the real source-shared allocations being retired.
    if(!owned->retiring||!canRetireNativeTriggerObservers())return false;
    auto& o=*owned;auto& pool=worldApplication_->garageObserverShared_;
    while(!o.links.empty()){auto* link=o.links.back().link;pc=0x105830;
        if(!pool.release105830(&o.persistent,link))return false;o.links.pop_back();}
    o.persistent.next00=nullptr;
    if(o.persistentConstructed){pc=0x280920;
        if(!pool.release280920(&o.persistent))return false;o.persistentConstructed=false;}
    if(o.temporaryConstructed){pc=0x280920;
        if(!pool.release280920(&o.temporary))return false;o.temporaryConstructed=false;}
    owned.reset();pc=0x2a0420;return true;
}
} // namespace ra_pod_scene50LA

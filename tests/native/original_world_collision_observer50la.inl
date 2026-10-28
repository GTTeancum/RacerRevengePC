// Included once by the actual World producer TU, outside any namespace.
#include "../../src/game/collection_bounds48s.h"
#include "../../src/game/portal_geometry50eo.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>
#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__)
#error "RA collision observer requires strict finite binary32 semantics"
#endif
static_assert(sizeof(float)==4 && std::numeric_limits<float>::is_iec559 &&
              std::numeric_limits<float>::digits==24,"RA collision observer requires binary32");
namespace ra_pod_scene50LA {
bool Owner::nativeCollisionPrimaryCurrent27B174(
    const g::OriginalWorldCollisionObjects50LA::NativeQuery& q)const noexcept {
    if(!runtimeQueries_||runtimeQueries_->sceneOwner_!=this||!sceneApplication_||
       q.application!=sceneApplication_.get()||!q.candidate||!q.source||
       !q.counted||q.retired||!q.loaded||q.resourceWithdrawn||!q.appended||
       q.primaryZone>=collision4C4_.size()||!q.primarySlot||
       !nativeCollisionListsCurrent())return false;
    const auto* s=runtimeQueries_->storage_.get();
    const auto* candidates=runtimeQueries_->candidates_.get();
    const auto i=q.resourceIndex,z=q.primaryZone;
    if(!worldCollisionObjects3CC_||!q.source->ordinal)return false;
    const auto* wrapper=worldCollisionObjects3CC_->nativeSlot(z,q.source->ordinal-1u);
    if(!wrapper||wrapper->nativeQuery.get()!=&q||
       !worldCollisionObjects3CC_->nativeQueryCurrent27B040(*wrapper)||
       q.entity.sessionGeneration!=sceneGeneration_)return false;
    if(!s||!s->borrowedWorldLease||!candidates||i>=candidates->candidates_.size()||
       &candidates->candidates_[i]!=q.candidate||i>=s->candidates.size()||
       s->candidates[i]!=q.candidate||i>=s->registrations.size()||
       i>=s->candidateZones.size()||i>=s->candidateMethods.size()||
       s->candidateZones[i]!=z+1u||s->candidateMethods[i]!=g::ZoneMethod50CL::First||
       q.source->zone!=z+1u||s->lifecycle.storage.registrations!=s->registrations.data()||
       s->lifecycle.storage.registries!=s->registries.data()||
       s->lifecycle.objectCount<=i||s->registries.size()!=20u||
       i>=s->objects.size()||i>=s->source.size())return false;
    const auto& native=q.candidate->candidate.collision;
    const auto& object=s->objects[i];const auto& state=s->source[i];
    if(object.identity!=native.filter.identity||object.typeTag!=native.typeTag||
       !object.hasShape||object.shapeTag!=0u||q.candidate->shapeType!=0u||object.shape0Ref||
       state.cachedRadius!=native.sphere.radius)return false;
    for(std::size_t axis=0;axis<3u;++axis)
        if(state.cachedCenter.v[axis]!=native.sphere.center.v[axis])return false;
    // Compact collector cache W is its original native projection metadata
    // (producer passes raw0), not source1202E0's output W1; do not conflate.
    const auto* method=collision4C4_[z].nativeList34;
    const auto& r=s->registrations[i];const auto& registry=s->registries[z*2u];
    const auto encoded=static_cast<std::uint32_t>((i+1u)*4u);
    return method&&method->world_==this&&method->registry_==&registry&&
        method->method_==&s->trees[z*2u]&&method->ordinal_==z*2u&&
        method->nodeReference_&&method->reference00_&&
        r.owner==&registry&&r.slot==q.primarySlot&&r.encodedObject==encoded&&
        registry.blockCount==1u&&registry.perBlock&&registry.perBlock<=1024u&&
        registry.lastUsed<=registry.perBlock&&r.slot<=registry.lastUsed&&
        r.slot<=registry.perBlock&&registry.words[r.slot-1u]==encoded;
}
bool Owner::canRetireNativeCollisionObserver27B174(
    const g::OriginalWorldCollisionObjects50LA::NativeQuery& q)const noexcept {
    if(q.registered&&!nativeCollisionPrimaryCurrent27B174(q))return false;
    if(!q.observerSharedConstructed)return !q.observerToken||
        (!q.observerToken->next00&&!q.observerToken->child04&&!q.observerToken->method08);
    if(!sceneApplication_||!q.observerToken)return false;
    const auto& pool=sceneApplication_->garageObserverShared_;
    return pool.owns(q.observerToken.get())&&!pool.ownerHasLinks(q.observerToken.get())&&
        !q.observerToken->next00;
}
bool Owner::retireNativeCollisionObserver27B174(
    g::OriginalWorldCollisionObjects50LA::NativeQuery& q)noexcept {
    if(!canRetireNativeCollisionObserver27B174(q))return false;
    if(q.observerSharedConstructed){
        if(!sceneApplication_->garageObserverShared_.release280920(q.observerToken.get()))return false;
        q.observerSharedConstructed=false;
    }
    q.observerToken.reset();
    // Only withdraw this source borrow. The original native primary remains
    // registered until the SAME Storage's existing teardown; never move it.
    q.registered=false;q.primaryZone=std::numeric_limits<std::size_t>::max();q.primarySlot=0;
    return true;
}
bool Owner::completeNativeCollisionPlacement27B174(
    std::size_t zone,std::size_t row,std::uint32_t& pc)noexcept {
    pc=0x27b174;
    auto* w=worldCollisionObjects3CC_?worldCollisionObjects3CC_->nativeSlot(zone,row):nullptr;
    if(!w||!worldCollisionObjects3CC_->nativeQueryCurrent27B040(*w)||!w->nativeQuery||
       !runtimeQueries_||runtimeQueries_->sceneOwner_!=this||!sceneApplication_)return false;
    auto& q=*w->nativeQuery;
    if(q.registered){pc=0x27b294;return nativeCollisionPrimaryCurrent27B174(q);}
    if(q.terminal||q.observerAttempted||!q.loaded||q.resourceWithdrawn||!q.appended||
       !q.candidate||!w->decodedCollision||!w->descriptor08||
       runtimeQueries_->closed_||runtimeQueries_->terminal_||runtimeQueries_->nativeFifoPins_||
       runtimeQueries_->triggerQueryActive_||runtimeQueries_->sharedOwnerLease_||
       runtimeQueries_->sharedRoleViews_||
       (runtimeQueries_->endCandidateProducer_&&runtimeQueries_->endCandidateProducer_->inCall)||
       sceneApplication_->attachmentLock423F58()||!nativeCollisionListsCurrent()||
       !canRetireNativePortalWorldLinks())return false;
    const auto reject=[&](std::uint32_t at)noexcept{pc=at;q.failurePC=at;return false;};
    const auto* s=runtimeQueries_->storage_.get();const auto* owners=runtimeQueries_->candidates_.get();
    const auto i=q.resourceIndex;
    if(!s||!owners||!owners->source_||owners->source_!=worldCollisionObjects3CC_->source_||
       !owners->source_->sourcesDecoded()||i>=owners->source_->sourcesDecoded()->collisions.size()||
       w->decodedCollision!=&owners->source_->sourcesDecoded()->collisions[i]||
       i>=owners->candidates_.size()||&owners->candidates_[i]!=q.candidate||
       i>=owners->radii_.size()||i>=s->candidates.size()||s->candidates[i]!=q.candidate||
       zone>=10u||q.source!=w->source||q.source->zone!=zone+1u||q.source->ordinal!=row+1u)
        return reject(pc);
    const auto& retained=*w->decodedCollision;const auto& candidate=*q.candidate;
    const auto& collision=candidate.candidate.collision;const auto& mesh=retained.mesh;
    const auto& radii=owners->radii_[i];
    if(candidate.shapeType!=0u||candidate.compound||mesh.nodes.empty()||
       collision.geometry.mesh!=&mesh||retained.member!=q.member||
       retained.zone!=zone+1u||retained.ordinal!=row+1u||
       candidate.nodeRadii!=radii.values.data()||candidate.nodeRadiusCount!=radii.values.size()||
       radii.values.size()!=mesh.nodes.size()||candidate.maxDepth!=radii.maxDepth||
       !worldCollisionResources_||!worldCollisionResources_->loadedBy11B920(i,q.entity))return reject(pc);
    for(std::size_t n=0;n<16u;++n){
        if(mesh.local_matrix[n]!=(n%5u==0u?1.f:0.f)||
           !std::isfinite(retained.instance.world.v[n])||
           std::memcmp(&collision.geometry.worldMatrix.v[n],&retained.instance.world.v[n],sizeof(float)))
            return reject(0x11f680);
    }
    for(std::size_t n=0;n<3u;++n)
        if(!std::isfinite(retained.instance.sphere.center.v[n])||
           std::memcmp(&collision.sphere.center.v[n],&retained.instance.sphere.center.v[n],sizeof(float)))
            return reject(0x11f820);
    if(!std::isfinite(retained.instance.sphere.radius)||retained.instance.sphere.radius<0.f||
       std::memcmp(&collision.sphere.radius,&retained.instance.sphere.radius,sizeof(float)))return reject(0x11f80c);
    // Actual prepareTrackCollider50AO -> prepareSpan already produced these
    // immutable outputs. Native adoption at BOTH27B270/27B27C, no replay of
    // geometry, matrix/sphere writes or claim to missing previousC0 history.
    q.placementAdopted=true;
    q.observerAttempted=true;pc=0x280890;
    try{q.observerToken=std::make_unique<g::OriginalWorldCollisionObjects50LA::NativeQuery::TemporaryObserver>();}
    catch(...){return reject(pc);}
    if(!sceneApplication_->garageObserverShared_.construct280890(q.observerToken.get()))return reject(pc);
    q.observerSharedConstructed=true;
    // Source120070 constructs the temporary BEFORE1202E0. Numeric failure
    // retains its actual pool owner for explicit native partial-load cleanup.
    pc=0x1202e0;if(std::fegetround()!=FE_TONEAREST)return reject(pc);
    g::CollectionBoundsInput48S input{};g::CollectionBoundsResult48S bounds{};
    // Numeric helper tag1 represents its root-box projection, NOT source
    // query shape1. The actual source/candidate shape is proven0 above.
    input.shapeTag=1u;input.shapeWorld=collision.geometry.worldMatrix;
    input.cachedCenter=retained.instance.sphere.center;input.cachedRadius=collision.sphere.radius;
    for(std::size_t n=0;n<3u;++n){input.localCenter.v[n]=mesh.nodes[0].center[n];input.localHalf.v[n]=mesh.nodes[0].half[n];}
    input.localCenter.v[3]=1.f;
    if(g::prepareCollectionBounds48S(input,bounds)!=g::CollectionBoundsStatus48S::Complete)return reject(pc);
    for(std::size_t n=0;n<3u;++n)
        if(!std::isfinite(bounds.center.v[n])||bounds.center.v[n]!=collision.sphere.center.v[n])return reject(pc);
    // Numeric equality intentionally admits signed zero; no universal EE/VU
    // bit equivalence or original FCSR history is asserted.
    pc=0x280b88;
    if(i>=s->registrations.size())return reject(pc);
    q.primaryZone=zone;q.primarySlot=s->registrations[i].slot;
    if(!nativeCollisionPrimaryCurrent27B174(q))return reject(pc);
    auto& observer=*q.observerToken;
    observer.child04=&collision4C4_[zone].entity;observer.method08=collision4C4_[zone].nativeList34;
    observer.slot14=q.primarySlot;observer.next00=nullptr;
    for(std::size_t n=0;n<4u;++n)observer.center20[n]=bounds.center.v[n]; //280B90..BBC.
    // SAME canonical portal geometry and reverse-authored chains. No new
    // membership, source E0 observer, List insertion or selected array.
    const auto& canonical=owners->source_->sourcesDecoded()->liveWorld;
    if(!s->borrowedWorldLease||s->world.portalHeads!=canonical.portalHeads||s->world.portalLinks!=canonical.portalLinks||
       s->world.portals!=canonical.portals||s->world.zoneCount!=10u||
       s->world.portalCount!=10u||s->world.portalLinkCount!=20u||
       !canonical.portalHeads||!canonical.portalLinks||!canonical.portals)return reject(0x280bf0);
    auto ref=s->world.portalHeads[zone];std::size_t traversed=0;
    for(auto* edge=collision4C4_[zone].portalLinks30.get();edge;edge=edge->next00.get()){
        pc=0x280c40;
        if(++traversed>10u||!ref||ref>s->world.portalLinkCount)return reject(pc);
        const auto& live=s->world.portalLinks[ref-1u];
        if(!live.portal||live.portal>nativePortals_.size()||live.portal>s->world.portalCount||
           !live.zone||live.zone>10u)return reject(pc);
        const auto& portal=nativePortals_[live.portal-1u];
        if(!portal||!portal->loaded||!portal->attached||portal->worldLinksPublished!=4u||
           !portal->geometry||edge->portal08!=&portal->entity||
           edge->neighbor04!=&collision4C4_[live.zone-1u].entity)return reject(pc);
        const auto* actualPortal=lighting_?lighting_->nativeRaPortal230450(this,
            owners->source_->sourcesDecoded()->world,visibilityRows_,sceneApplication_.get(),live.portal-1u,pc):nullptr;
        if(!actualPortal||portal->geometry!=actualPortal)return reject(pc);
        // Visibility Portal and collision liveWorld have distinct authentic
        // geometry allocations. Compare their actual prepared lanes, never
        // demand pointer equality or substitute one owner for the other.
        const auto& collisionPortal=s->world.portals[live.portal-1u];
        for(std::size_t axis=0;axis<3u;++axis){
            if(portal->geometry->normal.v[axis]!=collisionPortal.normal.v[axis])return reject(pc);
            for(std::size_t vertex=0;vertex<4u;++vertex)
                if(portal->geometry->vertices[vertex].v[axis]!=collisionPortal.vertices[vertex].v[axis]||
                   portal->geometry->edgeNormals[vertex].v[axis]!=collisionPortal.edgeNormals[vertex].v[axis])return reject(pc);
        }
        g::Vector3_48C center{{bounds.center.v[0],bounds.center.v[1],bounds.center.v[2]}};
        bool overlaps=false;float distance=0;pc=0x231150;
        if(g::overlapPortal231150_50EO(*portal->geometry,center,0.f,overlaps,distance)!=
           g::PortalGeometryStatus50EO::Complete)return reject(pc);
        // Conditional input-separation analysis supports this bounded miss route.
        // A real unexpected hit is UNSUPPORTED, never silently suppressed.
        if(overlaps)return reject(0x280c60);
        ref=live.next;
    }
    if(ref)return reject(0x280c40);
    pc=0x280920;
    if(!sceneApplication_->garageObserverShared_.release280920(q.observerToken.get()))return reject(pc);
    q.observerSharedConstructed=false;q.observerToken.reset();
    // Association with the already-existing exact primary, NOT a fresh
    // source List insertion receipt. Source temporary has genuinely retired.
    q.registered=true;pc=0x27b294;return nativeCollisionPrimaryCurrent27B174(q);
}
} // namespace ra_pod_scene50LA

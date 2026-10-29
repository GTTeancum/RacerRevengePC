#pragma once
#include "original_wreck_assets50la.h"
#include "original_racer_model_scene50la.h"
#include "original_wreck_query_prepare50la.h"
#include "original_wreck_type0_prepare50la.h"
#include "original_application_entities50la.h"
#include "original_wreck_physical50la.h"
#include "body_wrapper48h.h"
#include "live_zone_membership50en.h"
#include "query_prepare48o.h"
#include "type3_state48p.h"
#include "original_camera_world_query.h"
#include "collision_normal_mode50ja.h"
#include "original_wreck_type2_storage50la.h"
#include "original_wreck_type0_owners50la.h"
#include "original_wreck_type3_owners50la.h"
#include "original_wreck_exclusion50la.h"
#include "original_wreck_identity_list50la.h"
#include "original_wreck_category_list50la.h"
#include "original_wreck_scene_domain50la.h"
#include "original_wreck_piece_query_owner50la.h"
#include "original_wreck_contact50la.h"
#include <array>
#include <optional>
#include <algorithm>
#include <cstring>
#include <limits>
#include <memory>

namespace rr::game {
struct OriginalWreckMovementQueryResult50LA;

// Actual shared collision namespace and canonical collector slots. All three
// slots are reserved COLD for this particular piece, before any world/query
// workspace is sized. Slot reservation alone must not enter an active extent.
struct OriginalWreckQueryWorld50LA {
    ZoneLifecycle50CL* lifecycle=nullptr;
    LiveZoneWorld50EN* world=nullptr;
    LiveZoneWorkspace50EN* workspace=nullptr;
    CollectionObject50BX* objects=nullptr;
    CollectionObjectState50BX* states=nullptr;
    OriginalCameraWorldObjectCopies* copies=nullptr;
    OriginalCameraWorldGeometry* geometry=nullptr;
    std::size_t objectCount=0;
    std::array<CollectionRef50BX,3> refs{}; // E0,E4,E8, source order
    std::array<std::uint32_t,3> identities{};
    const std::uint32_t* physicsBodyCategory3E0718=nullptr;
    const std::uint8_t* collisionTypeGp8010=nullptr;
    const SpringVector4_48N* sentinel3E02C0=nullptr;
    std::uint8_t* globalValid3E0730=nullptr;
    // Actual shared world views used by the identical nullable-E4 121920
    //shape3 path. No private camera/world snapshot is made by this owner.
    OriginalCameraWorldInput worldQuery{};
    OriginalCameraWorldMutable mutableQuery{};
    OriginalCameraWorldWorkspace queryWorkspace{};
    CollisionNormalModeOwner50JA* normalMode3DF5B0=nullptr;
    // Same object-ref prefix of genuine static numerical owners already
    // retained by the track. Missing tail owners remain reached boundaries.
    Type2StaticCandidate49X* const* type2Targets=nullptr;
    std::size_t type2TargetCount=0;
    const std::uint8_t* groupTypeGp8014=nullptr;
    const std::uint32_t* skipCategory3E0728=nullptr;
    // Actual authored collection size, cold capacity only. E0 needs own E4/E8
    // plus every other piece E4. Existing one-piece owners retain capacity2.
    std::size_t groupPieceCapacity=1;
    // Optional actual shared domain, prepared cold for this session. It must
    // outlive all query owners. Null retains a private one-piece domain.
    OriginalWreckSceneDomain50LA* modelScenes=nullptr;
};
enum class OriginalWreckQueryStatus50LA : std::uint8_t {
    complete,binding,allocation,graph,preparation,registration
};
struct OriginalWreckQueryResult50LA {
    OriginalWreckQueryStatus50LA status=OriginalWreckQueryStatus50LA::binding;
    std::uint32_t sourcePC=0;
    bool preparationCommitted=false,registrationCommitted=false;
    LiveZoneResult50EN registration{};
};

enum class OriginalWreckQueryCallbackStatus50LA : std::uint8_t {complete,binding,continuation};
struct OriginalWreckQueryCallbackResult50LA {
    OriginalWreckQueryCallbackStatus50LA status=OriginalWreckQueryCallbackStatus50LA::binding;
    std::uint32_t sourcePC=0x2cb420u;
    OriginalWreckContactResult50LA callback{};
};
// Native owner of the THREE actual collision objects allocated by165E90.
// It owns fixed membership backing, exclusions and prepared shapes; geometry
// and the model are borrowed from the real immutable/mutable asset owner.
// It never borrows intact vehicle690/694, their caches or their memberships.
// No per-frame allocation, geometry replacement, identity remapping or lookup.
//
// Construction is cold and serial. The three collector definitions and state
// records are canonical in the shared world table, not private copied worlds.
// Their source-unwritten cold words are supplied in those fresh native records
// and preserved. The caller owns their native initialization policy. Source
// constructors only write their recovered fields. In particular radius80 is
// NOT silently set before an actual source preparation writes it.
//
// This is collision ownership, not the piece activation/visibility scheduler.
// The caller invokes methods at the actual22C6A0/166E40 continuation positions.
// The complete piece owner must dispatch E4 callback2CB420 and actual dynamic
// contacts before admitting a frame that can reach them. No callback success
// or empty-world/miss receipt is manufactured by this owner.
class OriginalWreckQueries50LA final {
    friend class OriginalWreckRacerStorage50LA;
    friend OriginalWreckMovementQueryResult50LA queryOriginalWreckMovement50LA(
        OriginalWreckQueries50LA&,const OriginalWreckComponent50LA&,
        const SpringVector4_48N&,const SpringVector4_48N&,std::uint32_t) noexcept;
public:
    enum class QueryCallback { none,contact294BB0 };
    enum class ResponseCallback { none,physics165920,wreck2CB420 };
    struct Query {
        // One source9C slot with two typed alternatives.2955F0 overwrites
        // E0's initial piece owner with the actual DC contact owner and
        // installs294BB0 in E4. E4/E8 retain their piece owner9C.
        OriginalWreckPieceQueryOwner50LA* owner9C=nullptr;
        OriginalWreckPhysical50LA* contactOwner9C=nullptr;
        QueryCallback queryE4=QueryCallback::none;
        RootBodyState48H* bodyA0=nullptr; // E8 source ctor leaves this null
        ResponseCallback responseE8=ResponseCallback::none;
        SpringVector4_48N constructorCopied40{},constructorCopied50{};
        SpringVector4_48N previousCenter70{};
        float distance84=0.f;
        std::uint32_t currentIdentity94=0;
        std::unique_ptr<CollectionZoneLink50BX[]> links;
        std::unique_ptr<LiveZoneEntry50EN[]> entries;
        LiveZoneMembership50EN membership;
        // E0 stores world80; E4 stores the complete actual shape0 state.
        BodyMatrix48G type2World80{};
        PodShapeState49W type0{};
        OriginalWreckType0Owners50LA type0Owners;
        PodShapeSource49W fullType0Source{}; //derived only from owned resource arrays
        // E8's exact one-pair shape. Its source11FF60 preparation is a no-op;
        //1202E0 still recomputes the registration sphere from these endpoints.
        SpringVector4_48N endpoints[2]{};
        std::uint32_t endpointMetadata[2]{};
        Type3QueryState48P type3{};
        OriginalWreckType3Owners50LA type3Owners;
        const SpringVector4_48N* movementEndpoints()const noexcept{return type3Owners.attempted()?type3Owners.endpoints():endpoints;}
        const std::uint32_t* movementMetadata()const noexcept{return type3Owners.attempted()?type3Owners.metadata():endpointMetadata;}
        OriginalCameraSingleRay ray{};
        std::uint32_t normalMode=0;
        std::unique_ptr<OriginalWreckType2Storage50LA> type2;
    };
    OriginalWreckQueries50LA()=default;
    OriginalWreckQueries50LA(const OriginalWreckQueries50LA&)=delete;
    OriginalWreckQueries50LA& operator=(const OriginalWreckQueries50LA&)=delete;
    OriginalWreckQueries50LA(OriginalWreckQueries50LA&&)=delete;
    OriginalWreckQueries50LA& operator=(OriginalWreckQueries50LA&&)=delete;

    // Native cold allocation and immutable numeric staging only. Publication
    // is split below so the owning165E90 continuation constructs/appends E0,
    // then DC/D8, then E4/E8 in source order. No canonical query definition,
    // state, copy word or shared invalidation byte is written here. Uses
    // actual application-owned named PHYSICSBODY bit, not 11B570's argument:
    //11B570(3/5) selects F8 flags, NOT the category. Exclusion order is exact.
    OriginalWreckQueryResult50LA prepareCold(OriginalWreckPieceQueryOwner50LA& piece,
        const OriginalWreckQueryWorld50LA& world,
        OriginalWreckComponent50LA& actualModelStorage,
        const std::array<OriginalWreckComponent50LA*,3>& actualQueryStorage) noexcept {
        return prepareColdImpl(piece,world,actualModelStorage,actualQueryStorage,true);
    }
    // Full canonical-model route: allocate/capture actual world backing only.
    // Initial model histories cannot be staged before genuine model loading.
    OriginalWreckQueryResult50LA prepareBackingCold(OriginalWreckPieceQueryOwner50LA& piece,
        const OriginalWreckQueryWorld50LA& world,OriginalWreckComponent50LA& actualModelStorage,
        const std::array<OriginalWreckComponent50LA*,3>& actualQueryStorage)noexcept {
        return prepareColdImpl(piece,world,actualModelStorage,actualQueryStorage,false);
    }
private:
    OriginalWreckQueryResult50LA prepareColdImpl(OriginalWreckPieceQueryOwner50LA& piece,
        const OriginalWreckQueryWorld50LA& world,OriginalWreckComponent50LA& actualModelStorage,
        const std::array<OriginalWreckComponent50LA*,3>& actualQueryStorage,bool stageModelShapes)noexcept {
        using S=OriginalWreckQueryStatus50LA;
        if(!piece.boundCold())return {S::binding,0x165e90u};
        auto& assets=piece.assets();auto& physical=piece.physical();const auto sessionGeneration=piece.sessionGeneration();
        if(retired_||storage_||!sessionGeneration||
           (world.modelScenes&&(!world.modelScenes->prepared()||world.modelScenes->sessionGeneration()!=sessionGeneration))||
           !world.groupPieceCapacity||world.groupPieceCapacity>512u||
           !assets.prepared()||!world.lifecycle||!world.world||!world.workspace||
           !world.objects||!world.states||!world.copies||!world.geometry||!world.objectCount||
           world.objectCount!=world.lifecycle->objectCount||!world.physicsBodyCategory3E0718||
           !*world.physicsBodyCategory3E0718||!world.collisionTypeGp8010||
           *world.collisionTypeGp8010==0xffu||!world.sentinel3E02C0||!world.globalValid3E0730||
           !world.world->zoneCount||!world.lifecycle->storage.registrations||
           world.lifecycle->storage.registrationCapacity<world.objectCount||
           world.world->portalCount==std::numeric_limits<std::size_t>::max())
            return {S::binding,0x165e90u};
        for(unsigned i=0;i<3;++i){
            if(!actualQueryStorage[i]||actualQueryStorage[i]==&actualModelStorage)return {S::binding,0x11bbc0u};
            for(unsigned j=0;j<i;++j)if(actualQueryStorage[i]==actualQueryStorage[j])return {S::binding,0x11bbc0u};
        }
        const SpringVector4_48N vf0{{0.f,0.f,0.f,1.f}};
        if(std::memcmp(world.sentinel3E02C0,&vf0,sizeof vf0))return {S::binding,0x11b444u};
        auto graph=assets.graph();const auto model=assets.model14C();
        const auto modelIndex=static_cast<std::int32_t>(model);
        if(!validateSceneGraph49P(graph)||modelIndex<0||static_cast<std::uint32_t>(modelIndex)>=graph.count)
            return {S::graph,0x165f10u};
        for(unsigned i=0;i<3;++i){
            if(!world.refs[i]||world.refs[i]>world.objectCount||!world.identities[i])return {S::binding,0x11b190u};
            const auto& registered=world.lifecycle->storage.registrations[world.refs[i]-1u];
            if(registered.owner||registered.slot||registered.encodedObject)return {S::binding,0x120070u};
            for(unsigned j=0;j<i;++j)if(world.refs[i]==world.refs[j]||world.identities[i]==world.identities[j])
                return {S::binding,0x11b190u};
            for(std::size_t j=0;j<world.objectCount;++j)
                if(j!=world.refs[i]-1u&&world.objects[j].identity==world.identities[i])return {S::binding,0x11b190u};
        }
        try {
            auto next=std::make_unique<Storage>();next->assets=&assets;next->world=world;
            next->plannedModel=&actualModelStorage;next->plannedQueries=actualQueryStorage;
            if(world.modelScenes)next->sceneDomain=world.modelScenes;
            else {if(!next->localScenes.prepareCold(sessionGeneration,1u))return {S::allocation,0x11bbc0u};
                next->sceneDomain=&next->localScenes;}
            next->piece=&piece;next->physical=&physical;
            next->capacity=world.world->portalCount+1u;
            const auto category=*world.physicsBodyCategory3E0718;
            next->exclusionCapacity[0]=world.groupPieceCapacity+1u;next->exclusionCapacity[1]=2u;
            for(unsigned i=0;i<2;++i)next->exclusions[i]=std::make_unique<std::uint32_t[]>(next->exclusionCapacity[i]);
            next->exclusions[0][0]=world.identities[1];next->exclusions[0][1]=world.identities[2];
            next->exclusions[1][0]=world.identities[0];next->exclusions[1][1]=world.identities[2];
            next->categoryD8[0]=category;
            auto& definitions=next->coldDefinitions;auto& states=next->coldStates;
            for(unsigned i=0;i<3;++i){
                auto& q=next->queries[i];q.owner9C=&piece;
                q.contactOwner9C=nullptr;q.queryE4=QueryCallback::none;
                q.bodyA0=i==2?nullptr:&physical.body.root;
                q.responseE8=i==1?ResponseCallback::physics165920:ResponseCallback::none;
                q.links=std::make_unique<CollectionZoneLink50BX[]>(next->capacity);
                q.entries=std::make_unique<LiveZoneEntry50EN[]>(next->capacity);
                q.membership.storage={q.links.get(),q.entries.get(),next->capacity};
                q.previousCenter70={{0.f,-10000.f,0.f,1.f}};
                q.type0.registrationE0=0u;
                const auto index=world.refs[i]-1u;
                definitions[i]=world.objects[index];states[i]=world.states[index];
                auto& object=definitions[i];auto& state=states[i];
                object.identity=world.identities[i];object.typeTag=*world.collisionTypeGp8010;
                object.group=0;object.shape0Ref=0;object.hasShape=true;object.shapeTag=i==0?2u:i==1?0u:3u;
                auto& f=object.filter;f.categoryA4=category;f.acceptMaskB8=0xffffffffu;
                f.flags31=static_cast<std::uint8_t>((f.flags31&0xf0u)|3u);
                for(auto& list:f.identities)list={nullptr,0,0,1};
                f.categories={nullptr,0,0,1};
                if(i==2)f.categories={next->categoryD8.data(),2,1,1};
                state.epoch=0;state.cachedCenter=vf0;
                //11B190 clears0/4/5, preserves6/7; 11B570 clears3 and
                // selects1/2. E8's explicit166384 additionally sets bit0.
                state.flagsF8=static_cast<std::uint8_t>((state.flagsF8&0xc0u)|(i==0?2u:i==1?4u:3u));
                if(i==2){
                    q.endpoints[0]=vf0;q.endpoints[1]=vf0;
                    q.type3.worldCenter=vf0;q.type3.queryCenter=vf0;
                    // The native queryRadius aliases collision80, which this
                    // source11C710 prefix has not initialized.
                    std::memcpy(&q.type3.queryRadius,&state.cachedRadius,sizeof state.cachedRadius);
                    q.type3.result00=vf0;q.type3.result10=vf0;
                    q.type3.selectedNormal=vf0;q.type3.selectedPoint=vf0;q.type3.selectedCorrection=vf0;
                    q.ray.worldStart=vf0;q.ray.worldDirection=vf0;q.ray.normal=vf0;q.ray.point=vf0;
                    // Other scalar ray-result fields are native fresh storage
                    // and are not exposed as a completed126F40 query.
                }
            }
            if(stageModelShapes){const auto initial=stageInitialModelShapes(*next);
                if(initial.status!=S::complete)return initial;}
            //11BBE8..11BD18 owns result90 and its pair arrays in THIS E0.
            // No second provider can own result90 for the same query. Scratch
            // derives from actual cold resource depths, never a frame budget.
            auto& type2=next->queries[0].type2;
            type2=std::make_unique<OriginalWreckType2Storage50LA>();
            auto& selected=type2->result90.selected;
            selected.first=vf0;selected.second=vf0;selected.normal=vf0;
            selected.surfacePoint=vf0;selected.correction=vf0;
            const auto& resource=assets.queryE0();
            if(world.type2TargetCount>world.objectCount||
               (world.type2TargetCount&&!world.type2Targets))return {S::binding,0x11bbe8u};
            std::size_t targetDepth=0;
            for(std::size_t i=0;i<world.type2TargetCount;++i){
                const auto* target=world.type2Targets[i];
                if(!target)return {S::binding,0x11bbe8u};
                if(target->shapeType==0)targetDepth=std::max(targetDepth,target->maxDepth);
                else if(target->shapeType==1){
                    const auto* compound=target->compound;
                    if(!compound||compound->count>maxCompoundMembers50IJ||
                       (compound->count&&!compound->parts))return {S::binding,0x11bbe8u};
                    for(std::size_t j=0;j<compound->count;++j)
                        targetDepth=std::max(targetDepth,compound->parts[j].maxDepth);
                }
            }
            if(resource.maxDepth==std::numeric_limits<std::size_t>::max()||
               targetDepth>std::numeric_limits<std::size_t>::max()-resource.maxDepth-1u)
                return {S::allocation,0x11bbe8u};
            type2->stack.resize(1u+resource.maxDepth+targetDepth);
            for(auto* bank:{&type2->live,&type2->banks[0],&type2->banks[1],&type2->scratch}){
                bank->pairs.resize(resource.rayCount);bank->metadata.resize(resource.rayCount);
            }
            // Only fixed native backing is retained here. Source publication
            // is performed by publishColdQuery at each actual ctor position.
            storage_=std::move(next);
            return {S::complete,0u};
        }catch(...){return {S::allocation,0x165ec8u};}
    }
public:
    OriginalWreckQueryResult50LA prepareInitialModelQueries11F570()noexcept {
        if(freshE0Backing())return {};
        using S=OriginalWreckQueryStatus50LA;
        if(!storage_||storage_->e0ConstructorReached||storage_->initialShapesAttempted||storage_->initialShapesReady||!storage_->piece->coherent()||
           storage_->published[0]||storage_->published[1]||storage_->published[2])return {S::binding,0x11f570u};
        storage_->initialShapesAttempted=true;return stageInitialModelShapes(*storage_);
    }
    bool prepared()const noexcept{return storage_&&!storage_->freshE0Backing;}
    bool initialModelQueriesPrepared()const noexcept{return storage_&&storage_->initialShapesReady;}
    bool hasFullE0PublicationHistory()const noexcept{return storage_&&storage_->e0FullPublication;}
    // Terminal native cancellation BEFORE the first source11B190 publication.
    // prepareCold owns backing only; no shared table field or Entity has been
    // published by this owner yet. This is not11B4A0 or active-piece retirement.
    // The world owner invokes it only after its actual terminal lifetime gates.
    bool canReleaseUnpublishedWorld(const LiveZoneWorld50EN& actualWorld)const noexcept {
        return canReleaseWorldBacking(actualWorld,false);
    }
    bool canReleaseSourceRetiredWorld(const LiveZoneWorld50EN& actualWorld)const noexcept {
        return canReleaseWorldBacking(actualWorld,true);
    }
private:
    bool canReleaseWorldBacking(const LiveZoneWorld50EN& actualWorld,bool sourceRetired)const noexcept {
        if(!storage_||storage_->freshE0Backing||retired_||storage_->world.world!=&actualWorld||
           storage_->contactBound||storage_->mutualBound||storage_->derivedBound||
           (storage_->collectionFilterApplied&&!storage_->fullCollectionAttempted))return false;
        if(sourceRetired){
            if(!storage_->e0FullPublication||!storage_->e0ConstructorRetired||
               !storage_->queries[0].type2||!storage_->queries[0].type2->sourceOwners.retired()||
               storage_->physical->modelSceneDomain||!storage_->plannedQueries[0]||
               storage_->plannedQueries[0]->reference00||
               storage_->plannedQueries[0]->kind!=OriginalWreckComponentKind50LA::entity141300||
               object(0).identity!=storage_->world.identities[0]||object(0).shapeTag!=2u||object(0).hasShape)return false;
        }else if(storage_->e0FullPublication)return false;
        if(storage_->fullCollectionAttempted&&(!fullCategoryOwnership()||
           !storage_->sourceCategoryD8[0].retired()||!storage_->sourceCategoryD8[1].retired()))return false;
        if(storage_->fullMutualAttempted&&(!fullMutualOwnership()||
           !storage_->sourceIdentityC0[0].retired()||!storage_->sourceIdentityC0[1].retired()))return false;
        const auto& w=storage_->world;
        if(storage_->e4FullPublication){
            const auto& g=w.geometry[w.refs[1]-1u];
            if(!sourceRetired||!storage_->e4ConstructorRetired||!storage_->queries[1].type0Owners.retired()||
               object(1).identity!=w.identities[1]||object(1).shapeTag||object(1).hasShape||
               g.mesh||g.compound||g.hasResponseCallbackE8)return false;
        }
        if(storage_->e8FullPublication&&(!sourceRetired||!storage_->e8ConstructorRetired||
           !storage_->queries[2].type3Owners.retired()||!storage_->sourceCategoryD8[1].retired()||
           object(2).identity!=w.identities[2]||object(2).shapeTag!=3u||object(2).hasShape))return false;
        if(!w.lifecycle||!w.lifecycle->storage.registrations||
           w.lifecycle->storage.registrationCapacity<w.objectCount)return false;
        for(unsigned i=0;i<3;++i){
            const auto& q=storage_->queries[i];
            if((q.type2&&q.type2->sourceOwners.attempted()&&!sourceRetired)||
               (q.type0Owners.attempted()&&!q.type0Owners.retired())||
               (q.type3Owners.attempted()&&!q.type3Owners.retired())||
               storage_->published[i]||storage_->components[i]||q.membership.attached||
               q.membership.object||q.membership.revision||q.type0.registrationE0||
               !w.refs[i]||w.refs[i]>w.objectCount)return false;
            const auto& primary=w.lifecycle->storage.registrations[w.refs[i]-1u];
            if(primary.owner||primary.slot||primary.encodedObject)return false;
            for(std::size_t j=0;j<storage_->capacity;++j){
                const auto& e=q.entries[j];
                if(e.used||e.registration.owner||e.registration.slot||e.registration.encodedObject)return false;
            }
        }
        return true;
    }
public:
    bool releaseSourceRetiredWorld(const LiveZoneWorld50EN& actualWorld)noexcept {
        if(!canReleaseSourceRetiredWorld(actualWorld))return false;
        // Source shape, Entity and domain lifetimes already ended. Preserve
        // every canonical table row; only private native backing is disposed.
        storage_.reset();retired_=true;return true;
    }
    bool releaseUnpublishedWorld(const LiveZoneWorld50EN& actualWorld)noexcept {
        if(!canReleaseUnpublishedWorld(actualWorld))return false;
        // Nothing in the canonical world was published, so do not clear or
        // restore any shared definition, selected hit, filter, or geometry.
        // Drop ALL nested world/target/workspace borrows with their backing.
        storage_.reset();retired_=true;return true;
    }
    bool retired()const noexcept{return retired_;}
    bool constructed()const noexcept {
        return storage_&&!storage_->freshE0Backing&&storage_->published[0]&&storage_->published[1]&&storage_->published[2]&&
            storage_->contactBound&&storage_->mutualBound&&storage_->derivedBound;
    }
    // Source165E90 constructor publication. Component141300 has already run
    // on this actual +8 subobject, so flags31 borrows its authoritative29.
    // Source-specific later fields (2955F0 and2CB5A0) are separate methods.
    OriginalWreckQueryResult50LA publishColdQuery(unsigned i,OriginalWreckComponent50LA& component,
        OriginalWreckComponent50LA& actualModel,SpringSceneId48N actualModelIdentity) noexcept {
        using S=OriginalWreckQueryStatus50LA;
        if(!storage_||storage_->freshE0Backing||storage_->e0ConstructorReached||i>2||storage_->published[i]||!storage_->initialShapesReady||!storage_->piece->coherent()||
           storage_->plannedModel!=&actualModel||storage_->plannedQueries[i]!=&component||
           component.kind!=OriginalWreckComponentKind50LA::collision11B190||
           !component.objectKey||!component.sessionGeneration||
           !storage_->physical->bodyConstructorFieldsApplied||
           (i&& !storage_->published[i-1u])||(i==1&&!storage_->contactBound))
            return {S::binding,0x165e90u};
        auto& w=storage_->world;const auto index=w.refs[i]-1u;
        const auto& registered=w.lifecycle->storage.registrations[index];
        if(registered.owner||registered.slot||registered.encodedObject)return {S::binding,0x11b190u};
        OriginalWreckSceneDomain50LA::Entry scenePublication{};
        if(i==0&&!storage_->sceneDomain->preparePublication(actualModelIdentity,*storage_->assets,
           *storage_->physical,actualModel,component,w.identities[0],scenePublication))
            return {S::binding,0x1253f0u};
        auto definition=storage_->coldDefinitions[i];definition.filter.flags31=component.flags29;
        w.objects[index]=definition;w.states[index]=storage_->coldStates[i];
        w.copies[index].copied40=*w.sentinel3E02C0;w.copies[index].copied50=*w.sentinel3E02C0;
        w.geometry[index]={};
        if(i==1)w.geometry[index]={&storage_->assets->targetE4().mesh,query(1).type0.composed00,nullptr,true};
        *w.globalValid3E0730=0; //Actual11B570 invalidates the same application cache byte.
        storage_->components[i]=&component;storage_->published[i]=true;
        if(i==0)storage_->modelScene44=storage_->sceneDomain->publish1253F0(scenePublication,w.objects[index]);
        return {S::complete,0u};
    }
    // Source model/shape identity is available immediately after E0 publication,
    // before DC/POINT/detector construction. No prepared-owner shortcut.
    SpringSceneId48N modelSceneIdentity()const noexcept {
        return storage_&&storage_->published[0]?storage_->sceneDomain->identity(storage_->modelScene44):SpringSceneId48N{};
    }
    const OriginalWreckSceneDomain50LA* modelSceneDomain()const noexcept {
        return storage_?storage_->sceneDomain:nullptr;
    }
    // Exact2955F0 final detector binding: E0.9C=DC, A0=DC.30 and E4=294BB0.
    bool bindContactQuery() noexcept {
        if(!coherent(0)||storage_->contactBound||!storage_->physical->contactConstructorFieldsApplied||
           storage_->physical->contactBody30!=&storage_->physical->body)return false;
        auto& q=query(0);q.owner9C=nullptr;q.contactOwner9C=storage_->physical;
        q.bodyA0=&storage_->physical->body.root;q.queryE4=QueryCallback::contact294BB0;
        storage_->contactBound=true;return true;
    }
    // Exact166390..1663D4 after all three real objects exist. Identity-list
    // order is E0:[E4,E8], E4:[E0,E8]; no future query is inserted early.
    bool bindMutualExclusions() noexcept {
        if(!coherent(0)||!coherent(1)||!coherent(2)||storage_->mutualBound)return false;
        for(unsigned i=0;i<2;++i)object(i).filter.identities[0]={storage_->exclusions[i].get(),storage_->exclusionCapacity[i],2,1};
        storage_->mutualBound=true;return true;
    }
    // Actual group123EB0/123FA0 target is the other piece's E4 object identity.
    // Both owners must retain the SAME canonical namespace. These primitives
    // do not certify group attach/detach, alter E4 lists, or rebind E0.A0.
    OriginalWreckExclusionStatus50LA addGroupE4Exclusion(const OriginalWreckQueries50LA& target)noexcept {
        if(!constructed()||!target.constructed()||!coherent(0)||!target.coherent(1)||
           storage_->world.world!=target.storage_->world.world||
           storage_->world.objects!=target.storage_->world.objects)
            return OriginalWreckExclusionStatus50LA::binding;
        return addOriginalWreckExclusion123EB0(object(0).filter.identities[0],storage_->exclusions[0].get(),
            storage_->exclusionCapacity[0],target.object(1).identity);
    }
    OriginalWreckExclusionStatus50LA removeGroupE4Exclusion(const OriginalWreckQueries50LA& target)noexcept {
        if(!constructed()||!target.constructed()||!coherent(0)||!target.coherent(1)||
           storage_->world.world!=target.storage_->world.world||
           storage_->world.objects!=target.storage_->world.objects)
            return OriginalWreckExclusionStatus50LA::binding;
        return removeOriginalWreckExclusion123FA0(object(0).filter.identities[0],storage_->exclusions[0].get(),
            storage_->exclusionCapacity[0],target.object(1).identity);
    }
    // Actual2CB5A0 after165C30 completes; the earlier base callback165920 is
    // never misrepresented as the derived response before this source store.
    bool bindDerivedResponse() noexcept {
        if(!coherent(1)||!storage_->published[2]||!storage_->mutualBound||storage_->derivedBound)return false;
        query(1).responseE8=ResponseCallback::wreck2CB420;storage_->derivedBound=true;return true;
    }
    //121D28/122878 dispatch candidate.E8(receiver=candidate,other=querying
    // object), after querying object's E4 callback. Caller owns that sequencing.
    // This bounded adapter accepts a real querying wreck E0/E4/E8 in the SAME
    // canonical world. Other body families require their actual typed resolver.
    OriginalWreckQueryCallbackResult50LA respondFromQuery2CB420(
        const OriginalWreckQueries50LA& actualQueryingOwner,unsigned queryingIndex)noexcept {
        using S=OriginalWreckQueryCallbackStatus50LA;OriginalWreckQueryCallbackResult50LA out;
        if(!coherent(1)||!storage_->derivedBound||query(1).responseE8!=ResponseCallback::wreck2CB420||
           !actualQueryingOwner.coherent(queryingIndex)||
           storage_->world.world!=actualQueryingOwner.storage_->world.world||
           storage_->world.objects!=actualQueryingOwner.storage_->world.objects||
           storage_->piece->sessionGeneration()!=actualQueryingOwner.storage_->piece->sessionGeneration())return out;
        auto& receiver=*query(1).owner9C;auto& physical=receiver.physical();
        if(!physical.contactConstructorFieldsApplied||query(1).bodyA0!=&physical.body.root)return out;
        const auto* other=actualQueryingOwner.query(queryingIndex).bodyA0;
        // Existing scalar leaf requires disjoint consumed body inputs. Do not
        // fabricate a null other or an enabled receiver to bypass that boundary.
        if(!receiver.fields().wake30&&!physical.body.enabled&&other==&physical.body.root)return out;
        out.callback=respondOriginalWreckContact50LA(receiver.fields().wake30,physical.body.enabled,
            query(1).bodyA0->body,physical.body.velocity70W,other?&other->body:nullptr,
            physical.generated.impact.restitutionC0);
        out.sourcePC=out.callback.sourcePC;
        out.status=out.callback.status==OriginalWreckContactStatus50LA::complete?S::complete:S::continuation;
        return out;
    }
    Query& query(unsigned i) noexcept{if(freshE0Backing())std::terminate();return storage_->queries[i];}
    const Query& query(unsigned i)const noexcept{if(freshE0Backing())std::terminate();return storage_->queries[i];}
    CollectionObject50BX& object(unsigned i) noexcept{if(freshE0Backing())std::terminate();return storage_->world.objects[storage_->world.refs[i]-1u];}
    const CollectionObject50BX& object(unsigned i)const noexcept{if(freshE0Backing())std::terminate();return storage_->world.objects[storage_->world.refs[i]-1u];}
    CollectionObjectState50BX& state(unsigned i) noexcept{if(freshE0Backing())std::terminate();return storage_->world.states[storage_->world.refs[i]-1u];}
    const CollectionObjectState50BX& state(unsigned i)const noexcept{if(freshE0Backing())std::terminate();return storage_->world.states[storage_->world.refs[i]-1u];}
    CollectionRef50BX ref(unsigned i)const noexcept{if(freshE0Backing())std::terminate();return storage_->world.refs[i];}
    OriginalCameraWorldObjectCopies& copies(unsigned i) noexcept{if(freshE0Backing())std::terminate();return storage_->world.copies[ref(i)-1u];}
    const OriginalCameraWorldObjectCopies& copies(unsigned i)const noexcept{if(freshE0Backing())std::terminate();return storage_->world.copies[ref(i)-1u];}
    const OriginalWreckQueryWorld50LA& worldBinding()const noexcept{if(freshE0Backing())std::terminate();return storage_->world;}

    //22C214..22C258 adds the actual named WATER bit4342A8 to E0 and E8
    //category exclusions after2CB5A0 completes.124170 preserves first-match
    //order and does not append a duplicate. Storage remains distinct perD8.
    bool applyCollectionWaterExclusion(const std::uint32_t& actualWater4342A8) noexcept {
        if(!coherent(0)||!coherent(2)||storage_->collectionFilterApplied||storage_->fullCollectionAttempted||
           storage_->e0FullPublication||!actualWater4342A8)return false;
        storage_->waterCategoryE0=actualWater4342A8;
        object(0).filter.categories={&storage_->waterCategoryE0,1,1,1};
        auto& categories=object(2).filter.categories;
        if(storage_->categoryD8[0]!=actualWater4342A8){storage_->categoryD8[1]=actualWater4342A8;categories.count=2;}
        storage_->collectionFilterApplied=true;return true;
    }

    // Source11FE50 for E0/E4; refreshes their actual model graph first. This
    // does not perform the later120230 or claim observer28EA20 completed.
    OriginalWreckQueryResult50LA prepareModelQuery(unsigned i) noexcept {
        using S=OriginalWreckQueryStatus50LA;
        if(!coherent(i)||i>1)return {S::binding,0x11fe50u};
        auto graph=storage_->assets->graph();const auto model=storage_->assets->model14C();
        if(!refreshSceneWorld49P(graph,model))return {S::graph,0x28b550u};
        const auto& world=graph.nodes[static_cast<std::int32_t>(model)].world100;
        auto& q=query(i);auto& cache=state(i);
        if(i==0){
            QuerySpherePrepareOutput48O out{};
            if(!prepareType2(storage_->assets->queryE0(),world,out))return {S::preparation,0x11f680u};
            q.type2World80=out.world;cache.cachedCenter=out.center;cache.cachedRadius=out.radius;
        }else{
            if(!preparePodShape50IZ(storage_->e4FullPublication?q.fullType0Source:storage_->assets->targetE4().source,world,q.type0))return {S::preparation,0x11f680u};
            cache.cachedCenter=q.type0.center60;cache.cachedRadius=q.type0.radius80;
            storage_->world.geometry[ref(1)-1u].world=q.type0.composed00;
        }
        return {S::complete,0u,true,false};
    }
    // Exact11CA80 source80/90 copy, including complete W lanes. Pair count is
    // authentically ONE; there is no endpoint inferred from the vehicle center.
    OriginalWreckQueryResult50LA stageMovement80_90(const SpringVector4_48N& start,
        const SpringVector4_48N& end) noexcept {
        using S=OriginalWreckQueryStatus50LA;if(!coherent(2))return {S::binding,0x11ca80u};
        const SpringVector4_48N source[2]={start,end};QueryPairStagingInput48O in{};
        in.pairCount=1;in.source=source;in.sourceCapacity=2;
        auto& q=query(2);auto& tag=object(2).shapeTag;
        auto* endpoints=q.type3Owners.attempted()?(q.type3Owners.loaded()?q.type3Owners.shape8C_->endpoints08.get():nullptr):q.endpoints;
        auto* metadata=q.type3Owners.attempted()?(q.type3Owners.loaded()?q.type3Owners.shape8C_->metadata04.get():nullptr):q.endpointMetadata;
        if(stageQueryPairs48O(in,endpoints,2,metadata,2,tag)!=QueryPairStagingStatus48O::Staged)
            return {S::preparation,0x11ca80u};
        return {S::complete,0u,true,false};
    }
private:
    // Existing121920 transaction reads/writes the actual full E8 owner. The
    //legacy one-piece owner keeps its explicitly admitted inline representation.
    bool readMovementProducerState(OriginalCameraWorldQueryState& stage)const noexcept {
        if(!coherent(2))return false;
        const auto& q=storage_->queries[2];
        stage.type3=q.type3;stage.ray=q.ray;
        return !q.type3Owners.attempted()||q.type3Owners.readProducerState(stage.type3,stage.ray);
    }
    bool commitMovementProducerState(const OriginalCameraWorldQueryState& stage,
        const OriginalCameraWorldResult& receipt)noexcept {
        if(!coherent(2)||receipt.status!=OriginalCameraWorldStatus::Complete)return false;
        auto& q=query(2);
        if(q.type3Owners.attempted()&&!q.type3Owners.commitProducerState(stage.type3,stage.ray,receipt))return false;
        q.type3=stage.type3;q.ray=stage.ray;return true;
    }
public:
    //120070 always uses method2 and initial overlap radius ZERO. Call three
    // times E0,E4,E8 after the real preceding visibility28E8A0, not before it.
    OriginalWreckQueryResult50LA attach(unsigned i,CollectionRef50BX zone) noexcept {
        using S=OriginalWreckQueryStatus50LA;if(!coherent(i)||query(i).membership.attached)
            return {S::binding,0x120070u};
        auto b=bounds(i);auto& q=query(i);auto& w=storage_->world;
        auto r=attachLiveZoneMembership50EN(*w.lifecycle,*w.world,q.membership,q.membership.storage,
            w.refs[i],zone,ZoneMethod50CL::Second,0.f,b,*w.workspace);
        OriginalWreckQueryResult50LA result{r.status==LiveZoneStatus50EN::Complete?S::complete:S::registration,
            r.status==LiveZoneStatus50EN::Complete?0u:0x120070u,false,r.status==LiveZoneStatus50EN::Complete,r};
        if(result.registrationCommitted){if(i==1)q.type0.registrationE0=w.refs[i];publishBounds(i);}
        return result;
    }
    // Actual120230, including real detach-no-op route, without clearing E0 to
    // force that route. Prepared numeric prefixes survive a later failure.
    OriginalWreckQueryResult50LA update(unsigned i) noexcept {
        using S=OriginalWreckQueryStatus50LA;if(!coherent(i))return {S::binding,0x120230u};
        auto b=bounds(i);auto& q=query(i);auto& w=storage_->world;
        auto r=updateLiveZoneMembership50EN(*w.lifecycle,*w.world,q.membership,b,*w.workspace);
        OriginalWreckQueryResult50LA result{r.status==LiveZoneStatus50EN::Complete?S::complete:S::registration,
            r.status==LiveZoneStatus50EN::Complete?0u:0x120230u,false,r.status==LiveZoneStatus50EN::Complete,r};
        if(result.registrationCommitted&&q.membership.attached)publishBounds(i);
        return result;
    }
    OriginalWreckQueryResult50LA remove(unsigned i) noexcept {
        using S=OriginalWreckQueryStatus50LA;if(!coherent(i))return {S::binding,0x120290u};
        auto& q=query(i);auto& w=storage_->world;
        auto r=removeLiveZoneMembership50EN(*w.lifecycle,*w.world,q.membership,*w.workspace);
        OriginalWreckQueryResult50LA result{r.status==LiveZoneStatus50EN::Complete?S::complete:S::registration,
            r.status==LiveZoneStatus50EN::Complete?0u:0x120290u,false,r.status==LiveZoneStatus50EN::Complete,r};
        if(result.registrationCommitted&&i==1)q.type0.registrationE0=0u;
        return result;
    }
    bool coherent(unsigned i)const noexcept {
        if(!storage_||storage_->freshE0Backing||i>2||!storage_->published[i]||!storage_->initialShapesReady||!storage_->piece->coherent()||
           (i==0&&!modelSceneIdentity().value))return false;
        const auto& w=storage_->world;const auto& q=query(i);
        if(object(i).identity!=w.identities[i]||object(i).shapeTag!=(i==0?2u:i==1?0u:3u)||
           object(i).filter.categoryA4!=*w.physicsBodyCategory3E0718||
           object(i).typeTag!=*w.collisionTypeGp8010||!object(i).hasShape)return false;
        if(storage_->e0FullPublication&&storage_->contactBound&&
           (!storage_->fullContactComponent||!storage_->fullContactComponent->reference00||
            !storage_->fullDetectorComponent||!storage_->fullDetectorComponent->reference00))return false;
        if(i==1&&storage_->e4FullPublication){
            const auto* resource=q.type0Owners.resource();const auto& g=w.geometry[w.refs[1]-1u];
            if(!q.type0Owners.loaded()||!resource||!resource->live()||g.mesh!=&resource->mesh()||
               g.compound||!g.hasResponseCallbackE8)return false;
        }
        const bool contact=i==0&&storage_->contactBound;
        if(q.owner9C!=(contact?nullptr:storage_->piece)||
           q.contactOwner9C!=(contact?storage_->physical:nullptr)||
           q.queryE4!=(contact?QueryCallback::contact294BB0:QueryCallback::none)||
           q.responseE8!=(i==1?(storage_->derivedBound?ResponseCallback::wreck2CB420:ResponseCallback::physics165920):ResponseCallback::none)||
           q.bodyA0!=(i==2?nullptr:&storage_->physical->body.root)||
           !storage_->components[i]||object(i).filter.flags31!=storage_->components[i]->flags29||
           q.membership.storage.links!=q.links.get()||q.membership.storage.entries!=q.entries.get()||
           q.membership.storage.capacity!=storage_->capacity)return false;
        if(q.membership.attached){
            if(q.membership.object!=w.refs[i]||q.membership.method!=ZoneMethod50CL::Second)return false;
        }else if(q.membership.object)return false;
        return i!=1||q.type0.registrationE0==(q.membership.attached?w.refs[i]:0u);
    }
private:
    struct Storage {
        // Fresh166018 prefix uses the SAME Query/numeric storage, but has no
        // future-shape allocations or model-domain publication. Absent-only
        // lanes below do not duplicate the canonical definition/body fields.
        bool freshE0Backing=false,freshE0Started=false,freshE0Counted=false,freshE0Withdrawn=false;
        std::uint32_t freshE0PC=0;
        bool freshLoadAttempted=false,freshLoadComplete=false;
        bool freshPrepareAttempted=false,freshPrepareComplete=false,freshAppended=false;
        Query** freshDetectorQueryField38=nullptr; //Native borrow of the actual detector source38 lane.
        std::optional<const void*> freshShape8C,freshResult90;
        const OriginalRacerSceneObject50LA* freshModel44=nullptr;
        std::optional<rr::assets::OriginalStandardModelFileId50LA> freshFileIdentityB0;

        std::optional<std::uint8_t> freshA8,freshA9;
        std::uint8_t freshFlags03KnownMask=0;
        std::uint8_t freshFlags03=0; //Native fresh opaque byte; source changes only low three bits.
        std::optional<std::int64_t> freshFileB0;
        std::optional<std::uint32_t> freshMethodFC,freshMethod10,freshEC,freshF0,freshF4,fresh98;
        //Absent-only E4 source lanes; canonical Query/definition/state[1]
        //remain the sole receiver/filter/vector/epoch storage.
        struct FreshE4 {
            bool started=false,counted=false,loadAttempted=false,loadComplete=false,retired=false,prepareAttempted=false,prepareComplete=false,appended=false;
            std::uint32_t pc=0;
            std::uint8_t flags03=0,flags03KnownMask=0;
            std::optional<std::uint8_t> a8,a9;
            std::optional<std::int64_t> fileB0;
            std::optional<std::uint32_t> methodFC,method10,ec,f0,f4,word98;
            std::optional<const void*> shape8C,result90;
            std::optional<rr::assets::OriginalStandardModelFileId50LA> fileIdentityB0;
            const OriginalRacerSceneObject50LA* model110=nullptr;
            const std::optional<OriginalWreckComponent50LA*>* publicationField=nullptr;
        } freshE4;
        struct FreshE8 {
            bool started=false,counted=false,appended=false,loadAttempted=false,loadComplete=false,
                categorySelected=false,categoryComplete=false,retired=false;
            std::uint32_t pc=0;
            std::uint8_t flags03=0,flags03KnownMask=0;
            std::optional<std::uint8_t> a8,a9;
            std::optional<std::int64_t> fileB0;
            std::optional<std::uint32_t> methodFC,method10,ec,f0,f4,word98;
            std::optional<const void*> shape8C,result90;
            const std::optional<OriginalWreckComponent50LA*>* publicationField=nullptr;
        } freshE8;
        bool freshMutualAttempted=false,freshMutualComplete=false,freshDerivedBound=false;
        bool freshCollectionAttempted=false,freshCollectionE0Selected=false;
        std::size_t freshCollectionCursor=0;
        const std::uint32_t* freshNamedWaterBit=nullptr; //Exact same App native namespace field, not guest mask bytes.
        std::uint32_t freshCollectionPC=0;
        std::array<bool,2> freshIdentitySelected{};
        std::size_t freshMutualCursor=0;
        std::uint32_t freshMutualPC=0;

        OriginalWreckComponent50LA* plannedModel=nullptr;
        std::array<OriginalWreckComponent50LA*,3> plannedQueries{};
        OriginalWreckSceneDomain50LA localScenes;
        OriginalWreckSceneDomain50LA* sceneDomain=nullptr;
        OriginalWreckSceneDomain50LA::Handle modelScene44{};
        ~Storage(){
            if(e0FullPublication&&!e0ConstructorRetired)std::terminate();
            if(sceneDomain)sceneDomain->revokeNative(modelScene44);
        }
        OriginalWreckAssets50LA* assets=nullptr;
        OriginalWreckPieceQueryOwner50LA* piece=nullptr;
        OriginalWreckPhysical50LA* physical=nullptr;
        OriginalWreckQueryWorld50LA world{};
        std::size_t capacity=0;
        std::array<std::unique_ptr<std::uint32_t[]>,2> exclusions;
        std::array<std::size_t,2> exclusionCapacity{};
        std::array<std::uint32_t,2> categoryD8{};
        std::uint32_t waterCategoryE0=0;
        bool collectionFilterApplied=false;
        bool fullMutualAttempted=false;
        std::size_t fullMutualCursor=0;
        std::array<OriginalWreckIdentityList50LA,2> sourceIdentityC0;
        bool e0ConstructorReached=false,e0ConstructorRetired=false;
        bool e0FullPublication=false,e0Prepared=false;
        bool e4ConstructorReached=false,e4ConstructorRetired=false;
        bool e4CallbacksBound=false,e4Prepared=false,e4FullPublication=false;
        bool e8ConstructorReached=false,e8ConstructorRetired=false,e8OwnerBound=false,e8FullPublication=false;
        std::array<OriginalWreckCategoryList50LA,2> sourceCategoryD8; //E0,E8
        bool nativeRacerPodAttempted=false,nativeRacerPodCommitted=false;
        std::uint32_t nativeRacerPod694Identity=0;
        bool fullCollectionAttempted=false;
        std::size_t fullCollectionCursor=0;
        std::uint32_t fullCollectionWater=0;
        const OriginalWreckComponent50LA* fullContactComponent=nullptr;
        const OriginalWreckComponent50LA* fullDetectorComponent=nullptr;
        bool published[3]{},contactBound=false,mutualBound=false,derivedBound=false,initialShapesReady=false,initialShapesAttempted=false;
        std::array<OriginalWreckComponent50LA*,3> components{};
        std::array<CollectionObject50BX,3> coldDefinitions{};
        std::array<CollectionObjectState50BX,3> coldStates{};
        std::array<Query,3> queries;
    };
    // Full-owner-only source prefix. These are the SAME reserved E0 fields,
    // still private until actual11BBC0 model/shape publication. Legacy cold
    // staging remains separate and cannot consume this unfinished constructor.
    // Native load22CEBC/D0/E4 calls120290 BEFORE later shape updates.
    // Admit only the genuine constructor-null E0 membership branch. Never
    // set initialShapesReady, clear a registration, or borrow another query.
    bool removeNativeConstructorNull120290(unsigned i)noexcept {
        if(!storage_||i>2u||!storage_->published[i]||!storage_->components[i]||
           storage_->components[i]->reference00!=1u||!storage_->e0FullPublication||
           !storage_->e4FullPublication||!storage_->e8FullPublication||
           storage_->e0ConstructorRetired||storage_->e4ConstructorRetired||storage_->e8ConstructorRetired||
           storage_->initialShapesReady||!storage_->fullCollectionAttempted||
           !storage_->collectionFilterApplied||!fullCategoryOwnership()||!fullMutualOwnership())return false;
        auto& s=*storage_;auto& w=s.world;auto& q=s.queries[i];
        if(!w.lifecycle||!w.world||!w.workspace||!w.refs[i]||w.refs[i]>w.objectCount||
           q.membership.attached||q.membership.object||q.membership.revision||q.type0.registrationE0||
           q.membership.storage.links!=q.links.get()||q.membership.storage.entries!=q.entries.get()||
           q.membership.storage.capacity!=s.capacity||object(i).identity!=w.identities[i])return false;
        const auto& r=w.lifecycle->storage.registrations[w.refs[i]-1u];
        if(r.owner||r.slot||r.encodedObject)return false;
        for(std::size_t n=0;n<s.capacity;++n){const auto& e=q.entries[n];
            if(e.used||e.registration.owner||e.registration.slot||e.registration.encodedObject)return false;}
        return removeLiveZoneMembership50EN(*w.lifecycle,*w.world,q.membership,*w.workspace).status==LiveZoneStatus50EN::Complete;
    }
    bool freshE0Backing()const noexcept{return storage_&&storage_->freshE0Backing;}
    bool reserveFreshE0Backing(OriginalWreckPieceQueryOwner50LA& piece,
        const OriginalWreckQueryWorld50LA& w,OriginalWreckComponent50LA& model,
        const std::array<OriginalWreckComponent50LA*,3>& slots)noexcept {
        //copies/geometry belong to later Follow; neither is consumed at this cut.
        if(retired_||storage_||!piece.coherent()||!w.world||!w.lifecycle||!w.objects||!w.states||
           !w.objectCount||w.objectCount!=w.lifecycle->objectCount||
           !w.lifecycle->storage.registrations||w.lifecycle->storage.registrationCapacity<w.objectCount||
           !w.physicsBodyCategory3E0718||!*w.physicsBodyCategory3E0718||!w.sentinel3E02C0||
           !w.globalValid3E0730||w.modelScenes)return false;
        for(unsigned i=0;i<3;++i){
            if(!slots[i]||slots[i]==&model||!w.refs[i]||w.refs[i]>w.objectCount||!w.identities[i])return false;
            const auto& r=w.lifecycle->storage.registrations[w.refs[i]-1u];
            if(r.owner||r.slot||r.encodedObject)return false;
            for(unsigned j=0;j<i;++j)if(slots[i]==slots[j]||w.refs[i]==w.refs[j]||w.identities[i]==w.identities[j])return false;
            for(std::size_t j=0;j<w.objectCount;++j)if(w.objects[j].identity==w.identities[i])return false;
        }
        try {
            auto next=std::make_unique<Storage>();next->freshE0Backing=true;
            next->piece=&piece;next->assets=&piece.assets();next->physical=&piece.physical();
            next->world=w;next->plannedModel=&model;next->plannedQueries=slots;
            // Retain native unwritten scalar lanes, without publishing source rows.
            for(unsigned i=0;i<3;++i){next->coldDefinitions[i]=w.objects[w.refs[i]-1u];next->coldStates[i]=w.states[w.refs[i]-1u];}
            storage_=std::move(next);return true;
        }catch(...){return false;}
    }
    void beginFreshE0Header11B190()noexcept {
        auto& s=*storage_;auto& d=s.coldDefinitions[0];
        s.freshE0Started=true;s.freshE0PC=0x11b208u;
        s.coldStates[0].epoch=0;d.typeTag=0xffu; //11B1AC: canonical source00.
        s.freshFlags03=std::uint8_t(s.freshFlags03&~1u);s.freshFlags03KnownMask|=1u;
        s.freshFlags03=std::uint8_t(s.freshFlags03&~2u);s.freshFlags03KnownMask|=2u;
        s.freshFlags03=std::uint8_t(s.freshFlags03&~4u);s.freshFlags03KnownMask|=4u;
    }
    OriginalWreckQueryResult50LA continueFreshE0Counted11B210(OriginalWreckComponent50LA& e,
        OriginalEntityTypeRegistry50LA& types,std::optional<OriginalWreckComponent50LA*>& publication)noexcept {
        using S=OriginalWreckQueryStatus50LA;
        auto& s=*storage_;auto& d=s.coldDefinitions[0];auto& st=s.coldStates[0];auto& q=s.queries[0];
        s.freshE0Counted=true;s.components[0]=&e;
        d.filter.flags31=e.flags29; //Same counted +8 Entity flags, not header03.
        s.freshMethodFC=0x3ccd00u;s.freshMethod10=0x3ccd08u;
        const SpringVector4_48N vf0{{0.f,0.f,0.f,1.f}};
        q.constructorCopied40=vf0;q.constructorCopied50=vf0;st.cachedCenter=vf0;q.previousCenter70=vf0;
        s.freshFileB0=-1;s.freshE0PC=0x11b270u;
        const auto t=types.collisionObject11B270();
        if(!t.okay()){s.freshE0PC=t.sourcePC;return {S::binding,t.sourcePC};}
        d.typeTag=t.type;d.hasShape=false;d.shapeTag=0;
        s.freshShape8C=nullptr;s.freshResult90=nullptr; //11B310/328, after actual88 store.
        //88/8C/90 are genuine null source owners, not preloaded geometry.
        st.flagsF8=std::uint8_t(st.flagsF8&~0x20u);
        q.queryE4=QueryCallback::none;q.responseE8=ResponseCallback::none;s.freshEC=0;s.freshF0=0;
        st.flagsF8=std::uint8_t(st.flagsF8&~1u);
        st.flagsF8=std::uint8_t((st.flagsF8&~2u)|2u);
        st.flagsF8=std::uint8_t((st.flagsF8&~4u)|4u);
        d.filter.acceptMaskB8=0xffffffffu;
        d.filter.identities[0].mode=1;d.filter.identities[1].mode=1;d.filter.categories.mode=1;
        d.filter.identities[0].values=nullptr;d.filter.identities[1].values=nullptr;
        d.filter.identities[0].count=0;d.filter.identities[1].count=0;
        d.filter.categories.values=nullptr;d.filter.categories.count=0;
        //Native capacities are bookkeeping, not extra source stores.
        d.filter.identities[0].capacity=0;d.filter.identities[1].capacity=0;d.filter.categories.capacity=0;
        q.currentIdentity94=0;s.fresh98=0;q.owner9C=nullptr;q.bodyA0=nullptr;d.filter.categoryA4=0;s.freshA8=0;s.freshA9=0;
        st.flagsF8=std::uint8_t(st.flagsF8&~0x10u);s.freshF4=0;
        q.constructorCopied40=*s.world.sentinel3E02C0;q.constructorCopied50=*s.world.sentinel3E02C0;
        q.distance84=0;q.previousCenter70={{0.f,-10000.f,0.f,1.f}};q.type0.registrationE0=0;
        s.e0ConstructorReached=true;s.freshE0PC=0x166034u;publication=&e;
        //Actual11B570(3), then166054/5C/68; no shape/load/preparation.
        st.flagsF8=std::uint8_t(st.flagsF8&~8u); //11B594
        st.flagsF8=std::uint8_t(st.flagsF8|2u); //11B5C0
        st.flagsF8=std::uint8_t(st.flagsF8&~4u); //11B5E4
        *s.world.globalValid3E0730=0; //11B5F4, same actual name-buffer byte0.
        d.filter.categoryA4=*s.world.physicsBodyCategory3E0718;
        q.owner9C=s.piece;q.bodyA0=&s.physical->body.root;
        s.freshE0PC=0x166074u;return {S::complete,0x166074u};
    }
    //Original3C9B90 calls11AE30("WATER") and stores4342A8 at3C9C40.
    //The caller supplies the SAME App's named native category bit: explicit
    //namespace adaptation, never a claim that host bit32 equals guest chronology.
    OriginalWreckQueryResult50LA constructFreshCollectionFilters22C21C(
        const std::uint32_t& actualWater4342A8)noexcept {
        using S=OriginalWreckQueryStatus50LA;
        if(!freshE0Backing()||retired_||storage_->freshE0Withdrawn||storage_->e0ConstructorRetired||
           storage_->freshCollectionAttempted||!actualWater4342A8||
           !storage_->freshMutualComplete||!storage_->freshDerivedBound||!storage_->freshAppended||
           !storage_->freshE4.appended||!storage_->freshE8.appended||!storage_->freshE8.categoryComplete||
           !storage_->piece->coherent()||!storage_->components[0]||!storage_->components[2])return {};
        auto& s=*storage_;auto& e0=*s.components[0];auto& e8=*s.components[2];
        if(e0.reference00!=1u||e8.reference00!=1u||&e0!=s.plannedQueries[0]||
           &e8!=s.plannedQueries[2]||e0.parent1C!=&s.piece->component()||
           e8.parent1C!=e0.parent1C||e0.kind!=OriginalWreckComponentKind50LA::collision11B190||
           !canRetireFreshE8(e8)||!s.sourceCategoryD8[1].matchesFreshCollection(e8,s.coldDefinitions[2].filter.categories))return {};
        auto& c0=s.coldDefinitions[0].filter.categories;auto& c8=s.coldDefinitions[2].filter.categories;
        if(c0.values||c0.count||c0.capacity||c0.mode!=1||
           c8.count!=1||!c8.values||c8.values[0]!=*s.world.physicsBodyCategory3E0718)return {};
        s.freshCollectionAttempted=true;s.freshNamedWaterBit=&actualWater4342A8;s.freshCollectionPC=0x22c21cu;
        if(!s.sourceCategoryD8[0].selectFreshCollectionMode1124160(e0,c0))return {S::binding,s.freshCollectionPC};
        s.freshCollectionE0Selected=true;s.freshCollectionPC=0x22c238u;
        if(!s.sourceCategoryD8[0].appendFreshCollection124170(c0,actualWater4342A8))return {S::allocation,s.freshCollectionPC};
        s.freshCollectionCursor=1u;s.freshCollectionPC=0x22c254u;
        if(!s.sourceCategoryD8[1].appendFreshCollection124170(c8,actualWater4342A8))return {S::allocation,s.freshCollectionPC};
        s.freshCollectionCursor=2u;s.freshCollectionPC=0x22c25cu;
        return {S::complete,s.freshCollectionPC}; //Before actual s2/s0 increments, not all pieces ready.
    }
    bool freshCollectionCategoryCurrent(unsigned slot,const OriginalWreckComponent50LA& component)const noexcept {
        if(!freshE0Backing()||slot>1u)return false;
        const auto& s=*storage_;
        if(!s.freshCollectionAttempted)return slot!=0u||!s.freshCollectionE0Selected;
        if(!s.freshNamedWaterBit||!*s.freshNamedWaterBit||s.freshCollectionCursor>2u)return false;
        if(slot==0u&&!s.freshCollectionE0Selected)return s.freshCollectionCursor==0u;
        const auto& view=s.coldDefinitions[slot?2u:0u].filter.categories;
        if(!s.sourceCategoryD8[slot].matchesFreshCollection(component,view))return false;
        const auto expected=slot?(1u+((s.freshCollectionCursor==2u&&
            *s.freshNamedWaterBit!=*s.world.physicsBodyCategory3E0718)?1u:0u)):(s.freshCollectionCursor?1u:0u);
        if(view.count!=static_cast<std::int32_t>(expected)||view.capacity!=expected)return false;
        if(expected&&view.values[0]!=(slot?*s.world.physicsBodyCategory3E0718:*s.freshNamedWaterBit))return false;
        return expected!=2u||view.values[1]==*s.freshNamedWaterBit;
    }
    OriginalWreckQueryResult50LA constructFreshMutualAndDerived166390()noexcept {
        using S=OriginalWreckQueryStatus50LA;
        if(!freshE0Backing()||retired_||storage_->freshMutualAttempted||
           !storage_->freshE8.categoryComplete||storage_->freshE8.pc!=0x166390u||
           !storage_->freshE8.appended||!storage_->freshE4.appended||!storage_->freshAppended||
           !storage_->piece->coherent())return {};
        auto& s=*storage_;
        //Native identity encoding: each real Query/component keeps its SAME
        //reserved ID. This does not publish collector rows or truncate pointers.
        for(unsigned i=0;i<3u;++i){
            const auto* e=s.components[i];const auto ref=s.world.refs[i];const auto id=s.world.identities[i];
            if(!e||e!=s.plannedQueries[i]||e->reference00!=1u||e->kind!=OriginalWreckComponentKind50LA::collision11B190||
               e->parent1C!=&s.piece->component()||e->sessionGeneration!=s.piece->component().sessionGeneration||
               !ref||ref>s.world.objectCount||!id||s.published[i]||
               s.world.objects[ref-1u].identity!=s.coldDefinitions[i].identity)return {};
            const auto& registration=s.world.lifecycle->storage.registrations[ref-1u];
            if(registration.owner||registration.slot||registration.encodedObject)return {};
            for(unsigned j=0;j<i;++j)if(s.components[j]==e||s.world.refs[j]==ref||s.world.identities[j]==id)return {};
        }
        if(s.queries[1].responseE8!=ResponseCallback::physics165920)return {};
        s.freshMutualAttempted=true;
        const std::uint32_t modePC[2]={0x166394u,0x1663a0u};
        for(unsigned i=0;i<2u;++i){
            s.freshMutualPC=modePC[i];
            if(!s.sourceIdentityC0[i].selectFreshCollectionMode1123EA0(*s.components[i],s.coldDefinitions[i].filter.identities[0]))
                return {S::binding,s.freshMutualPC};
            s.freshIdentitySelected[i]=true;
        }
        const unsigned owner[4]={0,0,1,1},target[4]={1,2,0,2};
        const std::uint32_t callPC[4]={0x1663acu,0x1663b8u,0x1663c4u,0x1663d0u};
        for(unsigned i=0;i<4u;++i){
            s.freshMutualPC=callPC[i];
            if(!s.sourceIdentityC0[owner[i]].appendFreshCollection123EB0(s.coldDefinitions[owner[i]].filter.identities[0],
                *s.components[target[i]],s.world.identities[target[i]]))return {S::allocation,s.freshMutualPC};
            ++s.freshMutualCursor;
        }
        s.freshMutualComplete=true;s.freshMutualPC=0x1663d8u;
        //Return through165C30 to the actual derived callback store2CB610.
        s.queries[1].responseE8=ResponseCallback::wreck2CB420;
        s.freshDerivedBound=true;s.freshMutualPC=0x2cb614u;
        return {S::complete,s.freshMutualPC};
    }
    void beginFreshE8Header11B190(const std::optional<OriginalWreckComponent50LA*>& publication)noexcept {
        auto& s=*storage_;auto& d=s.coldDefinitions[2];
        s.freshE8.publicationField=&publication;s.freshE8.started=true;s.freshE8.pc=0x11b208u;
        s.coldStates[2].epoch=0;d.typeTag=0xffu; //11B1AC: canonical source00.
        s.freshE8.flags03=std::uint8_t(s.freshE8.flags03&~1u);s.freshE8.flags03KnownMask|=1u;
        s.freshE8.flags03=std::uint8_t(s.freshE8.flags03&~2u);s.freshE8.flags03KnownMask|=2u;
        s.freshE8.flags03=std::uint8_t(s.freshE8.flags03&~4u);s.freshE8.flags03KnownMask|=4u;
    }
    OriginalWreckQueryResult50LA continueFreshE8Counted11B210(OriginalWreckComponent50LA& e,
        OriginalEntityTypeRegistry50LA& types,std::optional<OriginalWreckComponent50LA*>& publication)noexcept {
        using S=OriginalWreckQueryStatus50LA;
        auto& s=*storage_;auto& d=s.coldDefinitions[2];auto& st=s.coldStates[2];auto& q=s.queries[2];
        s.freshE8.counted=true;s.components[2]=&e;
        d.filter.flags31=e.flags29; //Same counted +8 Entity flags, not header03.
        s.freshE8.methodFC=0x3ccd00u;s.freshE8.method10=0x3ccd08u;
        const SpringVector4_48N vf0{{0.f,0.f,0.f,1.f}};
        q.constructorCopied40=vf0;q.constructorCopied50=vf0;st.cachedCenter=vf0;q.previousCenter70=vf0;
        s.freshE8.fileB0=-1;s.freshE8.pc=0x11b270u;
        const auto t=types.collisionObject11B270();
        if(!t.okay()){s.freshE8.pc=t.sourcePC;return {S::binding,t.sourcePC};}
        d.typeTag=t.type;d.hasShape=false;d.shapeTag=0;
        s.freshE8.shape8C=nullptr;s.freshE8.result90=nullptr; //11B310/328, after actual88 store.
        //88/8C/90 are genuine null source owners, not preloaded geometry.
        st.flagsF8=std::uint8_t(st.flagsF8&~0x20u);
        q.queryE4=QueryCallback::none;q.responseE8=ResponseCallback::none;s.freshE8.ec=0;s.freshE8.f0=0;
        st.flagsF8=std::uint8_t(st.flagsF8&~1u);
        st.flagsF8=std::uint8_t((st.flagsF8&~2u)|2u);
        st.flagsF8=std::uint8_t((st.flagsF8&~4u)|4u);
        d.filter.acceptMaskB8=0xffffffffu;
        d.filter.identities[0].mode=1;d.filter.identities[1].mode=1;d.filter.categories.mode=1;
        d.filter.identities[0].values=nullptr;d.filter.identities[1].values=nullptr;
        d.filter.identities[0].count=0;d.filter.identities[1].count=0;
        d.filter.categories.values=nullptr;d.filter.categories.count=0;
        //Native capacities are bookkeeping, not extra source stores.
        d.filter.identities[0].capacity=0;d.filter.identities[1].capacity=0;d.filter.categories.capacity=0;
        q.currentIdentity94=0;s.freshE8.word98=0;q.owner9C=nullptr;q.bodyA0=nullptr;d.filter.categoryA4=0;s.freshE8.a8=0;s.freshE8.a9=0;
        st.flagsF8=std::uint8_t(st.flagsF8&~0x10u);s.freshE8.f4=0;
        q.constructorCopied40=*s.world.sentinel3E02C0;q.constructorCopied50=*s.world.sentinel3E02C0;
        q.distance84=0;q.previousCenter70={{0.f,-10000.f,0.f,1.f}};q.type0.registrationE0=0;
        s.e8ConstructorReached=true;s.freshE8.pc=0x166300u;publication=&e;
        //Actual11B570(3); source append166324 comes BEFORE owner/category/payload.
        st.flagsF8=std::uint8_t(st.flagsF8&~8u); //11B594
        st.flagsF8=std::uint8_t(st.flagsF8|2u); //11B5C0 mode3
        st.flagsF8=std::uint8_t(st.flagsF8&~4u); //11B5E4 mode3
        *s.world.globalValid3E0730=0; //11B5F4, same actual name-buffer byte0.
        s.freshE8.pc=0x166310u;return {S::complete,0x166310u};
    }
    void noteFreshE8Appended166324()noexcept {
        storage_->freshE8.appended=true;storage_->freshE8.pc=0x16632cu;
    }
    OriginalWreckQueryResult50LA loadFreshE8Type3To166390()noexcept {
        using S=OriginalWreckQueryStatus50LA;
        if(!freshE0Backing()||retired_||!storage_->e8ConstructorReached||!storage_->freshE8.appended||
           storage_->freshE8.loadAttempted||storage_->freshE8.pc!=0x16632cu||!storage_->piece->coherent())return {};
        auto& s=*storage_;auto& f=s.freshE8;auto& q=s.queries[2];auto& d=s.coldDefinitions[2];
        auto* e=s.components[2];auto* previous=s.components[1];
        if(!e||!previous||!s.freshE4.appended||e->reference00!=1u||e->parent1C!=&s.piece->component()||
           e->previous10!=previous||e->next14||previous->next14!=e||q.type3Owners.attempted())return {};
        f.loadAttempted=true;q.owner9C=s.piece;f.pc=0x16633cu;
        d.filter.categoryA4=*s.world.physicsBodyCategory3E0718;f.pc=0x166348u;
        if(!q.type3Owners.allocateFreshWreck11C710(*s.world.sentinel3E02C0,f.shape8C,d.shapeTag,f.result90,*e)){
            if(f.shape8C&&*f.shape8C)d.hasShape=true;
            f.pc=q.type3Owners.sourceBoundary();return {S::allocation,f.pc};
        }
        d.hasShape=true;f.loadComplete=true;f.pc=0x166358u;
        auto& categories=s.sourceCategoryD8[1];
        if(!categories.selectFreshCollectionMode1124160(*e,d.filter.categories)){f.pc=0x124164u;return {S::binding,f.pc};}
        f.categorySelected=true;f.pc=0x124170u;
        if(!categories.appendFreshCollection124170(d.filter.categories,*s.world.physicsBodyCategory3E0718)){
            f.pc=0x124214u;return {S::allocation,f.pc};
        }
        f.categoryComplete=true;
        s.coldStates[2].flagsF8=std::uint8_t((s.coldStates[2].flagsF8&~1u)|1u); //16638C
        f.pc=0x166390u;return {S::complete,f.pc};
    }
    bool canRetireFreshE8(const OriginalWreckComponent50LA& e)const noexcept {
        if(!freshE0Backing())return false;
        const auto& s=*storage_;const auto& f=s.freshE8;const auto& q=s.queries[2];
        if(!f.started)return !q.type3Owners.attempted()&&!e.reference00&&e.kind==OriginalWreckComponentKind50LA::unbound;
        if(f.retired||s.e8FullPublication||s.published[2]||s.plannedQueries[2]!=&e||!f.publicationField||
           e.next14||e.firstChild18||q.membership.attached||q.membership.object||q.membership.revision)return false;
        const auto* parent=f.appended?&s.piece->component():nullptr;
        const auto* previous=f.appended?s.components[1]:nullptr;
        if(e.parent1C!=parent||e.previous10!=previous||
           (f.appended&&(!s.freshE4.appended||!parent->reference00||!previous||previous->reference00!=1u||
            previous->kind!=OriginalWreckComponentKind50LA::collision11B190||previous->parent1C!=parent||
            previous->next14!=&e||previous->sessionGeneration!=e.sessionGeneration)))return false;
        if(f.counted){if(s.components[2]!=&e||e.reference00!=1u||e.kind!=OriginalWreckComponentKind50LA::collision11B190)return false;}
        else if(s.components[2]||e.reference00||e.kind!=OriginalWreckComponentKind50LA::unbound)return false;
        if(s.e8ConstructorReached?(!*f.publicationField||**f.publicationField!=&e):bool(*f.publicationField))return false;
        if(s.e8ConstructorReached&&(q.owner9C!=(f.loadAttempted?s.piece:nullptr)||q.contactOwner9C||q.bodyA0||
           q.queryE4!=QueryCallback::none||q.responseE8!=ResponseCallback::none||
           s.coldDefinitions[2].filter.categoryA4!=(f.loadAttempted?*s.world.physicsBodyCategory3E0718:0u)))return false;
        if(q.type3Owners.attempted()&&!q.type3Owners.preflightFreshWreckPayload(
            e,f.shape8C,s.coldDefinitions[2].shapeTag,f.result90,parent,previous))return false;
        if(f.categorySelected&&!s.sourceCategoryD8[1].matchesFreshCollection(e,s.coldDefinitions[2].filter.categories))return false;
        if(s.freshCollectionAttempted&&!freshCollectionCategoryCurrent(1u,e))return false;
        return true;
    }
    void retireFreshE8(OriginalWreckComponent50LA& e,std::uint32_t& count)noexcept {
        auto& s=*storage_;auto& f=s.freshE8;auto& q=s.queries[2];
        const auto* parent=f.appended?&s.piece->component():nullptr;auto* previous=f.appended?e.previous10:nullptr;
        if(f.appended)previous->next14=nullptr; //1424E0, keep dying source links.
        if(f.counted){--e.reference00;
            if(f.categorySelected&&!s.sourceCategoryD8[1].releaseFreshCollectionAfterReferenceDrop11B51C(
                e,s.coldDefinitions[2].filter.categories))std::terminate();
            if(q.type3Owners.attempted()&&!q.type3Owners.releaseFreshWreckPayloadAfterReferenceDrop(
                e,f.shape8C,s.coldDefinitions[2].shapeTag,f.result90,parent,previous))std::terminate();
            e.kind=OriginalWreckComponentKind50LA::entity141300;--count;
        }
        f.retired=true;s.e8ConstructorRetired=true;
    }
    void beginFreshE4Header11B190(const std::optional<OriginalWreckComponent50LA*>& publication)noexcept {
        auto& s=*storage_;auto& d=s.coldDefinitions[1];
        s.freshE4.publicationField=&publication;s.freshE4.started=true;s.freshE4.pc=0x11b208u;
        s.coldStates[1].epoch=0;d.typeTag=0xffu; //11B1AC: canonical source00.
        s.freshE4.flags03=std::uint8_t(s.freshE4.flags03&~1u);s.freshE4.flags03KnownMask|=1u;
        s.freshE4.flags03=std::uint8_t(s.freshE4.flags03&~2u);s.freshE4.flags03KnownMask|=2u;
        s.freshE4.flags03=std::uint8_t(s.freshE4.flags03&~4u);s.freshE4.flags03KnownMask|=4u;
    }
    OriginalWreckQueryResult50LA continueFreshE4Counted11B210(OriginalWreckComponent50LA& e,
        OriginalEntityTypeRegistry50LA& types,std::optional<OriginalWreckComponent50LA*>& publication)noexcept {
        using S=OriginalWreckQueryStatus50LA;
        auto& s=*storage_;auto& d=s.coldDefinitions[1];auto& st=s.coldStates[1];auto& q=s.queries[1];
        s.freshE4.counted=true;s.components[1]=&e;
        d.filter.flags31=e.flags29; //Same counted +8 Entity flags, not header03.
        s.freshE4.methodFC=0x3ccd00u;s.freshE4.method10=0x3ccd08u;
        const SpringVector4_48N vf0{{0.f,0.f,0.f,1.f}};
        q.constructorCopied40=vf0;q.constructorCopied50=vf0;st.cachedCenter=vf0;q.previousCenter70=vf0;
        s.freshE4.fileB0=-1;s.freshE4.pc=0x11b270u;
        const auto t=types.collisionObject11B270();
        if(!t.okay()){s.freshE4.pc=t.sourcePC;return {S::binding,t.sourcePC};}
        d.typeTag=t.type;d.hasShape=false;d.shapeTag=0;
        s.freshE4.shape8C=nullptr;s.freshE4.result90=nullptr; //11B310/328, after actual88 store.
        //88/8C/90 are genuine null source owners, not preloaded geometry.
        st.flagsF8=std::uint8_t(st.flagsF8&~0x20u);
        q.queryE4=QueryCallback::none;q.responseE8=ResponseCallback::none;s.freshE4.ec=0;s.freshE4.f0=0;
        st.flagsF8=std::uint8_t(st.flagsF8&~1u);
        st.flagsF8=std::uint8_t((st.flagsF8&~2u)|2u);
        st.flagsF8=std::uint8_t((st.flagsF8&~4u)|4u);
        d.filter.acceptMaskB8=0xffffffffu;
        d.filter.identities[0].mode=1;d.filter.identities[1].mode=1;d.filter.categories.mode=1;
        d.filter.identities[0].values=nullptr;d.filter.identities[1].values=nullptr;
        d.filter.identities[0].count=0;d.filter.identities[1].count=0;
        d.filter.categories.values=nullptr;d.filter.categories.count=0;
        //Native capacities are bookkeeping, not extra source stores.
        d.filter.identities[0].capacity=0;d.filter.identities[1].capacity=0;d.filter.categories.capacity=0;
        q.currentIdentity94=0;s.freshE4.word98=0;q.owner9C=nullptr;q.bodyA0=nullptr;d.filter.categoryA4=0;s.freshE4.a8=0;s.freshE4.a9=0;
        st.flagsF8=std::uint8_t(st.flagsF8&~0x10u);s.freshE4.f4=0;
        q.constructorCopied40=*s.world.sentinel3E02C0;q.constructorCopied50=*s.world.sentinel3E02C0;
        q.distance84=0;q.previousCenter70={{0.f,-10000.f,0.f,1.f}};q.type0.registrationE0=0;
        s.e4ConstructorReached=true;s.freshE4.pc=0x16626cu;publication=&e;
        //Actual11B570(5), then166288; no owner/body/response binding yet.
        st.flagsF8=std::uint8_t(st.flagsF8&~8u); //11B594
        st.flagsF8=std::uint8_t(st.flagsF8&~2u); //11B5C0 mode5
        st.flagsF8=std::uint8_t(st.flagsF8|4u); //11B5E4 mode5
        *s.world.globalValid3E0730=0; //11B5F4, same actual name-buffer byte0.
        d.filter.categoryA4=*s.world.physicsBodyCategory3E0718;
        s.freshE4.pc=0x166294u;return {S::complete,0x166294u};
    }
    OriginalWreckQueryResult50LA loadFreshE4Type0NativeTransport(
        const OriginalRacerSceneObject50LA& actualModel,const std::string& actualRequest,
        const std::shared_ptr<rr::assets::OriginalStandardTextureResolver50LA>& resolver,
        const std::string& directory,std::array<std::uint8_t,256>& name,
        rr::assets::OriginalStandardTextureDiagnostic50LA& diagnostic)noexcept {
        using S=OriginalWreckQueryStatus50LA;
        if(!freshE0Backing()||retired_||!storage_->e4ConstructorReached||storage_->freshE4.loadAttempted||
           storage_->freshE4.pc!=0x166294u||!storage_->piece->coherent()||
           !actualModel.constructed||actualModel.released||&actualModel.entity()!=storage_->plannedModel||
           !actualModel.entity().reference00||!resolver||
           storage_->world.globalValid3E0730!=&name.front())return {};
        auto& s=*storage_;auto& f=s.freshE4;auto& q=s.queries[1];
        f.loadAttempted=true;f.model110=&actualModel;
        auto fail=[&](S status,std::uint32_t pc){f.pc=pc;return OriginalWreckQueryResult50LA{status,pc};};
        try {
            const auto expected=s.assets->targetFileIdentity();
            if(!directory.empty()||actualRequest!=s.assets->receipt().targetMember||
               actualRequest.size()>=name.size()||actualRequest.find('\0')!=std::string::npos)
                return fail(S::binding,0x11b920u);
            f.pc=0x11b9fcu;std::memcpy(name.data(),actualRequest.c_str(),actualRequest.size()+1u);
            rr::assets::OriginalStandardModelFileView50LA file;f.pc=0x11ba2cu;
            if(!resolver->lookupModel132820(actualRequest,rr::assets::OriginalStandardModelFileKind50LA::colType0,file,diagnostic)||
               file.id.resolver!=expected.resolver||file.id.row!=expected.row||!file.bytes||
               *file.bytes!=s.assets->originalMembers()[3]||file.animation1536F0||file.geometry240110)
                return fail(S::binding,0x11ba2cu);
            //Warm1254A0 bypasses the11BB84 fileB0 store. Only the cold branch
            //publishes this typed native file alternative, never a fake handle.
            if(!file.collision125014){f.fileIdentityB0=file.id;f.fileB0.reset();}
            const auto& target=s.assets->targetE4();
            s.coldDefinitions[1].shapeTag=0;
            const bool done=q.type0Owners.loadFreshWreck11B920(file,s.assets->originalMembers()[3],
                target.mesh,target.radii,actualModel,resolver,f.shape8C,f.result90,*s.components[1],q.fullType0Source.local40,diagnostic);
            if(f.shape8C&&*f.shape8C)s.coldDefinitions[1].hasShape=true;
            if(!done)return fail(S::allocation,q.type0Owners.sourceBoundary()?q.type0Owners.sourceBoundary():0x11b920u);
            f.loadComplete=true;f.pc=0x16629cu;return {S::complete,0x16629cu};
        }catch(...){return fail(S::allocation,f.pc?f.pc:0x166294u);}
    }
    bool canRetireFreshE4(const OriginalWreckComponent50LA& e)const noexcept {
        if(!freshE0Backing())return false;
        const auto& s=*storage_;const auto& f=s.freshE4;const auto& q=s.queries[1];
        if(!f.started)return !q.type0Owners.attempted()&&!e.reference00&&e.kind==OriginalWreckComponentKind50LA::unbound;
        const auto* next=s.freshE8.appended?s.components[2]:nullptr;
        if(f.retired||s.e4FullPublication||s.published[1]||s.e4Prepared||
           s.plannedQueries[1]!=&e||!f.publicationField||e.next14!=next||e.firstChild18||
           q.membership.attached||q.membership.object||q.membership.revision||q.type0.registrationE0)return false;
        const auto* parent=f.appended?&s.piece->component():nullptr;
        const auto* previous=f.appended?s.fullDetectorComponent:nullptr;
        if(e.parent1C!=parent||e.previous10!=previous||
           (f.appended&&(!f.prepareComplete||!parent->reference00||!previous||previous->reference00!=1u||
            previous->kind!=OriginalWreckComponentKind50LA::detector295290||previous->parent1C!=parent||
            previous->next14!=&e||previous->sessionGeneration!=e.sessionGeneration)))return false;
        if(s.freshE8.started&&(!s.plannedQueries[2]||!canRetireFreshE8(*s.plannedQueries[2])))return false;
        if(next&&(!f.appended||next->parent1C!=parent||next->previous10!=&e||next->reference00!=1u||next->next14))return false;
        if(s.freshIdentitySelected[1]&&!s.sourceIdentityC0[1].matchesFreshCollection(e,s.coldDefinitions[1].filter.identities[0]))return false;
        if(s.freshDerivedBound&&q.responseE8!=ResponseCallback::wreck2CB420)return false;
        if(f.counted){if(s.components[1]!=&e||e.reference00!=1u||e.kind!=OriginalWreckComponentKind50LA::collision11B190)return false;}
        else if(s.components[1]||e.reference00||e.kind!=OriginalWreckComponentKind50LA::unbound)return false;
        if(s.e4ConstructorReached?(!*f.publicationField||**f.publicationField!=&e):bool(*f.publicationField))return false;
        if(q.type0Owners.attempted()){
            if(!f.model110||!f.model110->constructed||f.model110->released||
               &f.model110->entity()!=s.plannedModel||!f.model110->entity().reference00||
               !q.type0Owners.preflightFreshWreckPayload(*f.model110,e,f.shape8C,f.result90,parent,previous,next))return false;
        }
        return true;
    }
    void retireFreshE4(OriginalWreckComponent50LA& e,std::uint32_t& count)noexcept {
        auto& s=*storage_;auto& f=s.freshE4;auto& q=s.queries[1];
        if(s.freshE8.started&&!s.freshE8.retired)std::terminate();
        const auto* parent=f.appended?&s.piece->component():nullptr;
        auto* previous=f.appended?e.previous10:nullptr;
        if(f.appended)previous->next14=nullptr; //1424E0; retain dying E4 parent/previous history.
        if(f.counted){--e.reference00;
            if(s.freshIdentitySelected[1]&&!s.sourceIdentityC0[1].releaseFreshCollectionAfterReferenceDrop11B4F4(
                e,s.coldDefinitions[1].filter.identities[0]))std::terminate();

            if(q.type0Owners.attempted()&&!q.type0Owners.releaseFreshWreckPayloadAfterReferenceDrop(
                *f.model110,e,f.shape8C,f.result90,parent,previous))std::terminate();
            e.kind=OriginalWreckComponentKind50LA::entity141300;--count;
        }
        f.retired=true;s.e4ConstructorRetired=true; //Source/numeric history retained through withdrawal.
    }
    OriginalWreckQueryResult50LA loadFreshE0Type2NativeTransport(
        const OriginalRacerSceneObject50LA& actualModel,const std::string& actualRequest,
        const std::shared_ptr<rr::assets::OriginalStandardTextureResolver50LA>& resolver,
        const std::string& actualDirectory36B0F0,std::array<std::uint8_t,256>& actualName3E0730,
        rr::assets::OriginalStandardTextureDiagnostic50LA& diagnostic)noexcept {
        using S=OriginalWreckQueryStatus50LA;
        if(!freshE0Backing()||retired_||!storage_->e0ConstructorReached||storage_->freshLoadAttempted||
           storage_->freshE0PC!=0x166074u||!resolver||!storage_->piece->coherent())return {};
        auto& s=*storage_;auto& q=s.queries[0];auto& definition=s.coldDefinitions[0];
        if(!actualModel.constructed||actualModel.released||&actualModel.entity()!=s.plannedModel||
           !actualModel.entity().reference00||!s.components[0]||s.components[0]->reference00!=1u||
           actualModel.entity().sessionGeneration!=s.components[0]->sessionGeneration||q.type2||
           !s.freshShape8C||*s.freshShape8C||!s.freshResult90||*s.freshResult90||
           s.world.globalValid3E0730!=&actualName3E0730.front())return {};
        s.freshLoadAttempted=true;s.freshModel44=&actualModel;
        auto fail=[&](S status,std::uint32_t pc){s.freshE0PC=pc;return OriginalWreckQueryResult50LA{status,pc};};
        try {
            //Native metadata only. No future result banks, scene-domain IDs,
            //collector copies/geometry, or query-prepared state are fabricated.
            q.type2=std::make_unique<OriginalWreckType2Storage50LA>();
            const auto expected=s.assets->queryFileIdentity();
            if(!actualDirectory36B0F0.empty()||actualRequest!=s.assets->receipt().queryMember||
               actualRequest.empty()||actualRequest.size()>=actualName3E0730.size()||
               actualRequest.find('\0')!=std::string::npos)return fail(S::binding,0x11b920u);
            //Bounded empty-directory/already-.col branch. Preserve original
            //request bytes, not a newly normalized stem or immutable receipt.
            s.freshE0PC=0x11b9fcu;
            std::memcpy(actualName3E0730.data(),actualRequest.c_str(),actualRequest.size()+1u);
            rr::assets::OriginalStandardModelFileView50LA file;s.freshE0PC=0x11ba2cu;
            if(!resolver->lookupModel132820(actualRequest,rr::assets::OriginalStandardModelFileKind50LA::colType2,file,diagnostic)||
               file.id.resolver!=expected.resolver||file.id.row!=expected.row||!file.bytes||
               *file.bytes!=s.assets->originalMembers()[2]||file.collision125014||file.animation1536F0||file.geometry240110)
                return fail(S::binding,0x11ba2cu);
            //Explicit existing native decoded-COL transport; NOT recovered
            //numeric FileManager handle/read/close or warm cache chronology.
            s.freshFileIdentityB0=file.id;s.freshFileB0.reset(); //Typed11BB84 alternative.
            definition.shapeTag=2u; //124FB8 before1253DC allocation.
            auto& payload=q.type2->sourceOwners;
            if(!payload.allocateFreshWreckShape1253DC(actualModel,s.freshShape8C,*s.components[0]))
                return fail(S::allocation,payload.sourceBoundary());
            definition.hasShape=true;
            if(!payload.loadFreshWreckShape126640(s.assets->queryE0())||
               !payload.allocateFreshWreckResult11BBE8(0u,s.freshResult90))
                return fail(S::allocation,payload.sourceBoundary());
            s.freshLoadComplete=true;s.freshE0PC=0x16607cu;
            return {S::complete,0x16607cu}; //Before actual11F570; no preparation.
        }catch(...){return fail(S::allocation,s.freshE0PC);}
    }
    OriginalWreckQueryResult50LA prepareFreshE411F570(const OriginalRacerSceneObject50LA& actualModel,
        const SceneGraph49P& graph)noexcept {
        using S=OriginalWreckQueryStatus50LA;
        if(!freshE0Backing()||retired_||!storage_->freshE4.loadComplete||storage_->freshE4.prepareAttempted||
           storage_->freshE4.pc!=0x16629cu||storage_->freshE4.model110!=&actualModel||
           !storage_->piece->coherent())return {};
        auto& s=*storage_;auto& f=s.freshE4;auto& q=s.queries[1];
        const auto* resource=q.type0Owners.resource();
        if(!actualModel.constructed||actualModel.released||&actualModel.entity()!=s.plannedModel||
           !actualModel.entity().reference00||!s.components[1]||s.components[1]->reference00!=1u||
           s.components[1]->parent1C||s.components[1]->previous10||s.components[1]->next14||
           !q.type0Owners.freshShape40Current(q.fullType0Source.local40)||!resource||!resource->live()||
           resource->root1C()!=0u||resource->mesh().nodes.empty()||
           !f.shape8C||*f.shape8C!=q.type0Owners.shape8C_.get()||!f.result90||*f.result90||
           q.type0Owners.shape8C_->racerModel110!=&actualModel||!validateSceneGraph49P(graph)||
           graph.nodes!=s.assets->modelGraphIdentity()||graph.count!=s.assets->modelGraphNodeCount()||
           actualModel.sceneIndex!=s.assets->model14C())return {};
        const auto index=static_cast<std::int32_t>(actualModel.sceneIndex);
        if(index<0||static_cast<std::uint32_t>(index)>=graph.count)return {};
        f.prepareAttempted=true;
        q.responseE8=ResponseCallback::physics165920;f.pc=0x1662a8u;
        q.owner9C=s.piece;f.pc=0x1662b0u;
        q.bodyA0=&s.physical->body.root;f.pc=0x1662bcu;
        f.pc=0x28b550u;
        auto& node=graph.nodes[index];
        //Exact bounded fresh Model root. Do not substitute a copied Scene or
        //transactionally recompute an unrepresented parent chain.
        if(node.parent180!=SceneNodeIndex49P::none){f.pc=0x28b5b8u;return {S::graph,f.pc};}
        if(!(node.sceneFlags190&1u)){
            for(unsigned i=0;i<16;i+=2){std::uint32_t words[2];
                std::memcpy(words,node.localC0.v+i,sizeof words);
                std::memcpy(node.world100.v+i,words,sizeof words); //28B5F4/5F8
            }
            node.sceneFlags190=static_cast<std::uint8_t>((node.sceneFlags190&~1u)|1u); //28B61C
        }
        BodyMatrix48G stack20;
        for(unsigned i=0;i<16;i+=2){std::uint32_t words[2];
            std::memcpy(words,node.world100.v+i,sizeof words);
            std::memcpy(stack20.v+i,words,sizeof words); //28B638/63C
        }
        auto& state=s.coldStates[1];
        if(!OriginalWreckSourceType0Prepare50LA::pass(false,stack20,q.fullType0Source.local40,
            resource->mesh().nodes[0],q.type0.composed00,q.type0.world80,q.type0.previousC0,
            state.cachedCenter,state.cachedRadius,f.pc))return {S::preparation,f.pc};
        //11EF24 invokes the distinct11F680 type0 pass on the SAME live matrices.
        if(!OriginalWreckSourceType0Prepare50LA::pass(true,stack20,q.fullType0Source.local40,
            resource->mesh().nodes[0],q.type0.composed00,q.type0.world80,q.type0.previousC0,
            state.cachedCenter,state.cachedRadius,f.pc))return {S::preparation,f.pc};
        f.prepareComplete=true;f.pc=0x1662c8u;return {S::complete,f.pc};
    }
    void noteFreshE4Appended1662DC()noexcept {
        storage_->freshE4.appended=true;storage_->freshE4.pc=0x1662e4u;
    }
    OriginalWreckQueryResult50LA prepareFreshE011F570(const OriginalRacerSceneObject50LA& actualModel,
        const SceneGraph49P& graph)noexcept {
        using S=OriginalWreckQueryStatus50LA;
        if(!freshE0Backing()||retired_||!storage_->freshLoadComplete||storage_->freshPrepareAttempted||
           storage_->freshE0PC!=0x16607cu||storage_->freshModel44!=&actualModel||
           !storage_->piece->coherent()||!storage_->queries[0].type2)return {};
        auto& s=*storage_;auto& payload=s.queries[0].type2->sourceOwners;
        if(!actualModel.constructed||actualModel.released||&actualModel.entity()!=s.plannedModel||
           !actualModel.entity().reference00||!s.components[0]||s.components[0]->reference00!=1u||
           s.components[0]->parent1C||s.components[0]->previous10||s.components[0]->next14||
           !payload.loaded()||!s.freshShape8C||*s.freshShape8C!=payload.shape8C_.get()||
           !s.freshResult90||*s.freshResult90!=payload.result90_.get()||
           payload.shape8C_->racerModel44!=&actualModel||!payload.shape8C_->nodes4C||!payload.shape8C_->nodeCount||
           payload.shape8C_->root40||!validateSceneGraph49P(graph)||
           graph.nodes!=s.assets->modelGraphIdentity()||graph.count!=s.assets->modelGraphNodeCount()||
           actualModel.sceneIndex!=s.assets->model14C())return {};
        const auto index=static_cast<std::int32_t>(actualModel.sceneIndex);
        if(index<0||static_cast<std::uint32_t>(index)>=graph.count)return {};
        s.freshPrepareAttempted=true;s.freshE0PC=0x28b550u;
        auto& node=graph.nodes[index];
        //Exact bounded fresh Model root. Do not substitute a copied Scene or
        //transactionally recompute an unrepresented parent chain.
        if(node.parent180!=SceneNodeIndex49P::none){s.freshE0PC=0x28b5b8u;return {S::graph,s.freshE0PC};}
        if(!(node.sceneFlags190&1u)){
            for(unsigned i=0;i<16;i+=2){std::uint32_t words[2];
                std::memcpy(words,node.localC0.v+i,sizeof words);
                std::memcpy(node.world100.v+i,words,sizeof words); //28B5F4/5F8
            }
            node.sceneFlags190=static_cast<std::uint8_t>((node.sceneFlags190&~1u)|1u); //28B61C
        }
        BodyMatrix48G stackA0;
        for(unsigned i=0;i<16;i+=2){std::uint32_t words[2];
            std::memcpy(words,node.world100.v+i,sizeof words);
            std::memcpy(stackA0.v+i,words,sizeof words); //28B638/63C
        }
        auto& state=s.coldStates[0];
        if(!OriginalWreckSourceType2Prepare50LA::pass(false,stackA0,payload.shape8C_->nodes4C[0],
            payload.shape8C_->matrix00,state.cachedCenter,state.cachedRadius,s.freshE0PC))
            return {S::preparation,s.freshE0PC};
        //Actual11F1FC invokes11F680: second real pass, not output replay.
        if(!OriginalWreckSourceType2Prepare50LA::pass(true,stackA0,payload.shape8C_->nodes4C[0],
            payload.shape8C_->matrix00,state.cachedCenter,state.cachedRadius,s.freshE0PC))
            return {S::preparation,s.freshE0PC};
        s.freshPrepareComplete=true;s.freshE0PC=0x166084u;
        return {S::complete,0x166084u}; //No source append or world registration yet.
    }
    void noteFreshE0Appended166098()noexcept {
        storage_->freshAppended=true;storage_->freshE0PC=0x1660a0u;
    }
    bool borrowFreshE0PointResource(const OriginalRacerSceneObject50LA& actualModel,
        const OriginalWreckComponent50LA& actualE0,Type2ResourceView49X& out)const noexcept {
        if(!freshE0Backing()||retired_||!storage_->freshLoadComplete||!storage_->freshPrepareComplete||
           !storage_->piece->coherent()||storage_->queries[0].owner9C!=storage_->piece||
           storage_->queries[0].bodyA0!=&storage_->physical->body.root||
           !storage_->freshAppended||storage_->freshE0PC!=0x1660a0u||storage_->freshE0Withdrawn||storage_->freshModel44!=&actualModel||
           !actualModel.constructed||actualModel.released||&actualModel.entity()!=storage_->plannedModel||
           !actualModel.entity().reference00||storage_->components[0]!=&actualE0||actualE0.reference00!=1u||
           actualE0.parent1C!=&storage_->piece->component()||actualE0.next14||actualE0.firstChild18||
           !actualE0.previous10||actualE0.previous10->next14!=&actualE0||
           actualE0.previous10->kind!=OriginalWreckComponentKind50LA::gravity282670||
           actualE0.previous10->parent1C!=actualE0.parent1C||!storage_->queries[0].type2)return false;
        const auto& p=storage_->queries[0].type2->sourceOwners;
        if(!p.loaded()||!storage_->freshShape8C||*storage_->freshShape8C!=p.shape8C_.get()||
           !storage_->freshResult90||*storage_->freshResult90!=p.result90_.get()||
           p.shape8C_->racerModel44!=&actualModel)return false;
        out=p.resource();return true; //Borrow of actual owned arrays; no prepared/SceneId grant.
    }
    bool bindFreshContact2955F0(const OriginalWreckComponent50LA& contact,
        const OriginalWreckComponent50LA& detector,Query*& actualDetector38)noexcept {
        if(!freshE0Backing()||retired_||!storage_->freshAppended||!storage_->freshPrepareComplete||
           storage_->contactBound||storage_->e0ConstructorRetired||actualDetector38||
           !storage_->physical->contactConstructorFieldsApplied||
           storage_->physical->contactBody30!=&storage_->physical->body)return false;
        auto& s=*storage_;auto& q=s.queries[0];const auto* e0=s.components[0];
        const auto* root=&s.piece->component();
        if(!s.piece->coherent()||!e0||e0->reference00!=1u||e0->parent1C!=root||e0->next14!=&contact||
           contact.reference00!=1u||contact.kind!=OriginalWreckComponentKind50LA::contact295A40||
           contact.parent1C!=root||contact.previous10!=e0||contact.next14||contact.firstChild18||
           detector.reference00!=1u||detector.kind!=OriginalWreckComponentKind50LA::detector295290||
           detector.parent1C||detector.previous10||detector.next14||detector.firstChild18||
           contact.sessionGeneration!=root->sessionGeneration||detector.sessionGeneration!=root->sessionGeneration||
           q.owner9C!=s.piece||q.contactOwner9C||q.bodyA0!=&s.physical->body.root||q.queryE4!=QueryCallback::none)return false;
        s.freshDetectorQueryField38=&actualDetector38; //Native lifetime identity, not source alias store.
        actualDetector38=&q; //2955F0 source detector38 FIRST.
        q.owner9C=nullptr;q.contactOwner9C=s.physical; //same source9C typed alternative.
        q.bodyA0=&s.physical->body.root;q.queryE4=QueryCallback::contact294BB0;
        s.coldDefinitions[0].filter.identities[0].mode=1; //123EA0 sourceBC.
        s.fullContactComponent=&contact;s.fullDetectorComponent=&detector;s.contactBound=true;
        return true; //No callback execution/table registration or prepared grant.
    }
    bool canRetireFreshE0(const OriginalWreckComponent50LA& e,
        const std::optional<OriginalWreckComponent50LA*>& publication,
        const OriginalWreckComponent50LA* parent=nullptr,const OriginalWreckComponent50LA* previous=nullptr,
        const OriginalWreckComponent50LA* expectedNext=nullptr,
        const OriginalWreckComponent50LA* expectedAfterNext=nullptr,
        const OriginalWreckComponent50LA* expectedAfterDetectorE4=nullptr,
        const OriginalWreckComponent50LA* expectedAfterE4E8=nullptr)const noexcept {
        if(!freshE0Backing())return false;
        const auto& s=*storage_;
        if(bool(parent)!=bool(previous)||bool(parent)!=s.freshAppended||
           (parent&&(parent!=&s.piece->component()||!s.freshPrepareComplete||
             previous->kind!=OriginalWreckComponentKind50LA::gravity282670||previous->parent1C!=parent||
             previous->next14!=&e)))return false;
        if(expectedNext&&(!parent||expectedNext->reference00!=1u||
           expectedNext->kind!=OriginalWreckComponentKind50LA::contact295A40||
           expectedNext->parent1C!=parent||expectedNext->previous10!=&e||expectedNext->next14!=expectedAfterNext||
           expectedNext->sessionGeneration!=e.sessionGeneration))return false;
        if(expectedAfterNext&&(!expectedNext||expectedAfterNext->reference00!=1u||
           expectedAfterNext->kind!=OriginalWreckComponentKind50LA::detector295290||
           expectedAfterNext->parent1C!=parent||expectedAfterNext->previous10!=expectedNext||expectedAfterNext->next14!=expectedAfterDetectorE4||
           expectedAfterNext->sessionGeneration!=e.sessionGeneration))return false;
        if(expectedAfterDetectorE4&&(!expectedAfterNext||!s.freshE4.appended||!s.freshE4.prepareComplete||
           s.components[1]!=expectedAfterDetectorE4||expectedAfterDetectorE4->reference00!=1u||
           expectedAfterDetectorE4->kind!=OriginalWreckComponentKind50LA::collision11B190||
           expectedAfterDetectorE4->parent1C!=parent||expectedAfterDetectorE4->previous10!=expectedAfterNext||
           expectedAfterDetectorE4->next14!=expectedAfterE4E8||expectedAfterDetectorE4->firstChild18||
           expectedAfterDetectorE4->sessionGeneration!=e.sessionGeneration))return false;
        if(s.freshE4.appended&&!expectedAfterDetectorE4)return false;
        if(expectedAfterE4E8&&(!expectedAfterDetectorE4||!s.freshE8.appended||s.components[2]!=expectedAfterE4E8||
           expectedAfterE4E8->reference00!=1u||expectedAfterE4E8->kind!=OriginalWreckComponentKind50LA::collision11B190||
           expectedAfterE4E8->parent1C!=parent||expectedAfterE4E8->previous10!=expectedAfterDetectorE4||
           expectedAfterE4E8->next14||expectedAfterE4E8->firstChild18||
           expectedAfterE4E8->sessionGeneration!=e.sessionGeneration))return false;
        if(s.freshE8.appended&&!expectedAfterE4E8)return false;

        if(s.contactBound){
            if(!s.fullContactComponent||!s.fullDetectorComponent||!s.freshDetectorQueryField38||
               *s.freshDetectorQueryField38!=&s.queries[0]||s.fullContactComponent!=expectedNext||
               s.fullContactComponent->reference00!=1u||s.fullDetectorComponent->reference00!=1u||
               s.fullDetectorComponent->kind!=OriginalWreckComponentKind50LA::detector295290||
               s.fullDetectorComponent->sessionGeneration!=e.sessionGeneration||
               (expectedAfterNext?s.fullDetectorComponent!=expectedAfterNext:
                 (s.fullDetectorComponent->parent1C||s.fullDetectorComponent->previous10||s.fullDetectorComponent->next14))||
               s.coldDefinitions[0].filter.identities[0].mode!=1||
               s.queries[0].owner9C||s.queries[0].contactOwner9C!=s.physical||
               s.queries[0].bodyA0!=&s.physical->body.root||s.queries[0].queryE4!=QueryCallback::contact294BB0)return false;
        }
        if(s.e0FullPublication||s.e0Prepared||s.initialShapesAttempted||s.initialShapesReady||
           s.e0ConstructorRetired||s.published[0]||s.published[1]||s.published[2]||s.sceneDomain)return false;
        for(unsigned i=0;i<3;++i){const auto& q=s.queries[i];
            const auto& r=s.world.lifecycle->storage.registrations[s.world.refs[i]-1u];
            if(r.owner||r.slot||r.encodedObject||s.world.objects[s.world.refs[i]-1u].identity!=s.coldDefinitions[i].identity||
               q.membership.attached||q.membership.object||q.membership.revision||(i&&q.type2)||(q.type0Owners.attempted()&&!(i==1u&&s.freshE4.started))||(q.type3Owners.attempted()&&!(i==2u&&s.freshE8.started)))return false;
        }
        if(s.freshE4.started&&(!s.plannedQueries[1]||!canRetireFreshE4(*s.plannedQueries[1])))return false;
        if(s.freshE8.started&&(!s.plannedQueries[2]||!canRetireFreshE8(*s.plannedQueries[2])))return false;
        if(s.freshIdentitySelected[0]&&!s.sourceIdentityC0[0].matchesFreshCollection(e,s.coldDefinitions[0].filter.identities[0]))return false;
        if(s.freshCollectionAttempted&&!freshCollectionCategoryCurrent(0u,e))return false;
        if(s.queries[0].type2){
            if(!s.freshLoadAttempted||!s.freshModel44||&s.freshModel44->entity()!=s.plannedModel||
               !s.freshModel44->constructed||s.freshModel44->released)return false;
            const auto& payload=s.queries[0].type2->sourceOwners;
            if(payload.attempted()){
                if(!payload.preflightFreshWreckPayload(*s.freshModel44,e,s.freshShape8C,s.freshResult90,parent,previous,expectedNext,expectedAfterNext,expectedAfterDetectorE4,expectedAfterE4E8))return false;
            }else if(!s.freshShape8C||*s.freshShape8C||!s.freshResult90||*s.freshResult90)return false;
        }else if(s.freshLoadComplete)return false;
        if(s.freshE0Counted){
            if(e.reference00!=1u||e.kind!=OriginalWreckComponentKind50LA::collision11B190||s.components[0]!=&e)return false;
        }else if(e.reference00||e.kind!=OriginalWreckComponentKind50LA::unbound||s.components[0])return false;
        return e.parent1C==parent&&e.previous10==previous&&e.next14==expectedNext&&!e.firstChild18&&
            (s.e0ConstructorReached?(publication&&*publication==&e):!publication);
    }
    void retireFreshE0(OriginalWreckComponent50LA& e,std::uint32_t& count,
        const OriginalWreckComponent50LA* parent=nullptr,const OriginalWreckComponent50LA* previous=nullptr)noexcept {
        auto& s=*storage_;
        if(s.freshE4.started&&!s.freshE4.retired)std::terminate();
        if(s.freshE8.started&&!s.freshE8.retired)std::terminate();
        if(s.contactBound&&(!s.fullContactComponent||!s.fullDetectorComponent||
           s.fullContactComponent->reference00||s.fullDetectorComponent->reference00||
           s.fullContactComponent->kind!=OriginalWreckComponentKind50LA::entity141300||
           s.fullDetectorComponent->kind!=OriginalWreckComponentKind50LA::entity141300))std::terminate();
        // Before11B2FC a registry failure has no complete query destructor.
        // Cancel only the actual counted Entity prefix; no fake11B4A0 return.
        if(s.freshE0Counted){
            --e.reference00;
            if(s.freshIdentitySelected[0]&&!s.sourceIdentityC0[0].releaseFreshCollectionAfterReferenceDrop11B4F4(
                e,s.coldDefinitions[0].filter.identities[0]))std::terminate();
            //11B51C releases actual D8 after C0/C8 and before payload11B600.
            //Retain source pointer/count/mode as history through native withdrawal.
            if(s.freshCollectionE0Selected&&!s.sourceCategoryD8[0].releaseFreshCollectionAfterReferenceDrop11B51C(
                e,s.coldDefinitions[0].filter.categories))std::terminate();
 //Actual142590 reference drop before payload lifetime.
            if(s.queries[0].type2&&s.queries[0].type2->sourceOwners.attempted()){
                if(!s.queries[0].type2->sourceOwners.releaseFreshWreckPayloadAfterReferenceDrop(
                    *s.freshModel44,e,s.freshShape8C,s.freshResult90,parent,previous))std::terminate();
            }
            e.kind=OriginalWreckComponentKind50LA::entity141300;--count;
        }
        //No-load failures take the knownNULL payload branch. Reached shape
        //prefixes use their exact source release or explicit native cancellation
        //above. Registry failure cancels only its counted Entity prefix.
        s.e0ConstructorRetired=true;retired_=true;
        //Keep canonical fields and source aliases as history through withdrawal.
    }
    bool canWithdrawFreshE0()const noexcept {
        if(!freshE0Backing())return true;
        const auto& s=*storage_;
        if(s.freshE4.started&&(!s.freshE4.retired||(s.queries[1].type0Owners.attempted()&&!s.queries[1].type0Owners.retired())))return false;
        if(s.freshE8.started&&(!s.freshE8.retired||(s.queries[2].type3Owners.attempted()&&!s.queries[2].type3Owners.retired())))return false;
        if(!s.e0ConstructorRetired||!retired_||s.e0FullPublication||s.e0Prepared)return false;
        if(s.freshCollectionE0Selected&&!s.sourceCategoryD8[0].retired())return false;
        if(s.freshE8.categorySelected&&!s.sourceCategoryD8[1].retired())return false;
        if(s.queries[0].type2&&s.queries[0].type2->sourceOwners.attempted()&&!s.queries[0].type2->sourceOwners.retired())return false;
        return !s.freshE0Counted||(s.components[0]&&!s.components[0]->reference00&&
            s.components[0]->kind==OriginalWreckComponentKind50LA::entity141300);
    }
    void withdrawFreshE0()noexcept {
        if(!freshE0Backing()||storage_->freshE0Withdrawn)return;
        //End only native namespace borrows after whole-owner preflight. Retain
        //the source field history until the actual owning backing is released.
        storage_->world={};storage_->freshE0Withdrawn=true;
    }
    bool canConstructE0Prefix11B190()const noexcept {
        if(!storage_||storage_->freshE0Backing||storage_->e0ConstructorReached||storage_->initialShapesReady||
           storage_->initialShapesAttempted||!storage_->piece->coherent()||
           storage_->components[0]||storage_->published[0]||storage_->published[1]||storage_->published[2])return false;
        const auto& w=storage_->world;const auto& q=storage_->queries[0];
        const auto& registration=w.lifecycle->storage.registrations[w.refs[0]-1u];
        return !registration.owner&&!registration.slot&&!registration.encodedObject&&!q.membership.attached&&
            w.collisionTypeGp8010&&*w.collisionTypeGp8010==storage_->coldDefinitions[0].typeTag&&
            w.groupTypeGp8014&&*w.groupTypeGp8014!=0xffu&&
            w.physicsBodyCategory3E0718&&*w.physicsBodyCategory3E0718&&w.globalValid3E0730;
    }
    bool constructE0Prefix11B190(OriginalWreckComponent50LA& component,
        std::optional<OriginalWreckComponent50LA*>* nativePublication=nullptr)noexcept {
        if(nativePublication&&nativePublication->has_value())return false;
        if(!canConstructE0Prefix11B190()||storage_->plannedQueries[0]!=&component||
           component.kind!=OriginalWreckComponentKind50LA::collision11B190||component.reference00!=1u)return false;
        auto& s=*storage_;auto& definition=s.coldDefinitions[0];auto& state=s.coldStates[0];
        //11B190 owns88=0,8C=NULL,90=NULL. Cold native future-shape backing
        // is not a source allocation or completed11BBC0 call.
        definition.hasShape=false;definition.shapeTag=0u;
        definition.filter.flags31=component.flags29;
        definition.filter.categoryA4=0u;
        state.flagsF8=static_cast<std::uint8_t>((state.flagsF8&0xc0u)|6u);
        auto& query=s.queries[0];query.owner9C=nullptr;query.contactOwner9C=nullptr;query.bodyA0=nullptr;
        query.queryE4=QueryCallback::none;query.responseE8=ResponseCallback::none;
        query.constructorCopied40=*s.world.sentinel3E02C0;query.constructorCopied50=*s.world.sentinel3E02C0;
        query.previousCenter70={{0.f,-10000.f,0.f,1.f}};
        query.distance84=0.f;query.currentIdentity94=0;
        if(nativePublication)*nativePublication=&component; //166034 AFTER actual11B190 defaults.
        //16603C ->11B570(3): clear bit3 and select bits1/2=1/0.
        state.flagsF8=static_cast<std::uint8_t>((state.flagsF8&~0x0eu)|2u);
        *s.world.globalValid3E0730=0;
        definition.filter.categoryA4=*s.world.physicsBodyCategory3E0718; //166050
        query.owner9C=s.piece;query.bodyA0=&s.physical->body.root; //166058/166068
        s.components[0]=&component;s.e0ConstructorReached=true;return true;
    }
    SpringSceneId48N nativeModelIdentityForPublication()const noexcept {
        if(freshE0Backing())return {};
        if(!storage_||!storage_->e0ConstructorReached||storage_->e0ConstructorRetired||
           storage_->e0FullPublication||!storage_->components[0]||!storage_->plannedModel)return {};
        const auto& s=*storage_;
        return s.sceneDomain->nativeIdentityForPublication(*s.assets,*s.physical,*s.plannedModel,
            *s.components[0],s.world.identities[0]);
    }
    OriginalWreckQueryResult50LA publishFullE0Type2(SpringSceneId48N actualModel)noexcept {
        if(freshE0Backing())return {};
        using S=OriginalWreckQueryStatus50LA;
        if(!storage_||!storage_->e0ConstructorReached||storage_->e0ConstructorRetired||
           storage_->published[0]||storage_->e0FullPublication||!storage_->piece->coherent())return {S::binding,0x166074u};
        auto& s=*storage_;auto& q=s.queries[0];
        if(!q.type2||q.type2->sourceOwners.attempted()||!s.components[0]||s.components[0]->reference00!=1u||
           !canRetireE0Prefix11B4A0(*s.components[0]))return {S::binding,0x11bbc0u};
        OriginalWreckSceneDomain50LA::Entry entry{};
        if(!s.sceneDomain->preparePublication(actualModel,*s.assets,*s.physical,*s.plannedModel,
           *s.components[0],s.world.identities[0],entry))return {S::binding,0x1253f0u};
        auto& owned=q.type2->sourceOwners;
        if(!owned.allocateShape1253DC(actualModel))return {S::allocation,owned.sourceBoundary()};
        const auto index=s.world.refs[0]-1u;
        auto definition=s.coldDefinitions[0];definition.hasShape=true;definition.shapeTag=2u;
        s.world.objects[index]=definition;s.world.states[index]=s.coldStates[0];
        s.world.copies[index].copied40=q.constructorCopied40;s.world.copies[index].copied50=q.constructorCopied50;
        s.world.geometry[index]={};
        s.modelScene44=s.sceneDomain->publish1253F0(entry,s.world.objects[index]);
        s.published[0]=true;s.e0FullPublication=true;
        //166078 supplies actual a3=0. Loading stays on this same retained COL.
        if(!owned.loadShape126640(s.assets->queryE0())||!owned.allocateResult11BBE8(0u))
            return {S::allocation,owned.sourceBoundary()};
        return {S::complete,0x16607cu};
    }
    OriginalWreckQueryResult50LA prepareFullE011F570()noexcept {
        if(freshE0Backing())return {};
        using S=OriginalWreckQueryStatus50LA;
        if(!storage_||!storage_->e0FullPublication||storage_->e0Prepared||
           storage_->e0ConstructorRetired||!modelSceneIdentity().value)return {S::binding,0x11f570u};
        auto& s=*storage_;auto& q=s.queries[0];auto& owned=q.type2->sourceOwners;
        if(!owned.loaded())return {S::binding,owned.sourceBoundary()};
        auto graph=s.assets->graph();const auto model=s.assets->model14C();
        if(!refreshSceneWorld49P(graph,model))return {S::graph,0x28b550u};
        const auto& world=graph.nodes[static_cast<std::int32_t>(model)].world100;
        QuerySpherePrepareOutput48O out{};
        if(!prepareType2(owned.resource(),world,out))return {S::preparation,0x11f680u};
        owned.shape8C_->matrix00=out.world;q.type2World80=out.world;
        state(0).cachedCenter=out.center;state(0).cachedRadius=out.radius;
        s.e0Prepared=true;return {S::complete,0x166098u,true,false};
    }
    bool canConstructE4Prefix11B190()const noexcept {
        if(!storage_||!storage_->e0FullPublication||!storage_->e0Prepared||!storage_->contactBound||
           storage_->e0ConstructorRetired||storage_->e4ConstructorReached||storage_->initialShapesReady||
           storage_->published[1]||storage_->published[2]||storage_->components[1]||!modelSceneIdentity().value)return false;
        const auto& s=*storage_;const auto& q=s.queries[1];
        const auto& r=s.world.lifecycle->storage.registrations[s.world.refs[1]-1u];
        return !r.owner&&!r.slot&&!r.encodedObject&&!q.membership.attached&&!q.membership.object&&
            !q.membership.revision&&!q.type0Owners.attempted()&&s.fullDetectorComponent&&
            s.fullDetectorComponent->reference00==1u&&s.fullContactComponent&&s.fullContactComponent->reference00==1u;
    }
    bool constructE4Prefix11B190(OriginalWreckComponent50LA& component,
        std::optional<OriginalWreckComponent50LA*>* nativePublication=nullptr)noexcept {
        if(nativePublication&&nativePublication->has_value())return false;
        if(!canConstructE4Prefix11B190()||storage_->plannedQueries[1]!=&component||
           component.kind!=OriginalWreckComponentKind50LA::collision11B190||component.reference00!=1u)return false;
        auto& s=*storage_;auto& definition=s.coldDefinitions[1];auto& state=s.coldStates[1];auto& q=s.queries[1];
        definition.hasShape=false;definition.shapeTag=0;definition.filter.flags31=component.flags29;
        definition.filter.categoryA4=0;state.flagsF8=static_cast<std::uint8_t>((state.flagsF8&0xc0u)|6u);
        q.owner9C=nullptr;q.contactOwner9C=nullptr;q.bodyA0=nullptr;
        q.queryE4=QueryCallback::none;q.responseE8=ResponseCallback::none;
        q.constructorCopied40=*s.world.sentinel3E02C0;q.constructorCopied50=*s.world.sentinel3E02C0;
        q.previousCenter70={{0.f,-10000.f,0.f,1.f}};q.distance84=0.f;q.currentIdentity94=0;
        if(nativePublication)*nativePublication=&component; //16626C AFTER actual11B190 defaults.
        //166274 ->11B570(5); actual category follows at166288.
        state.flagsF8=static_cast<std::uint8_t>((state.flagsF8&~0x0eu)|4u);*s.world.globalValid3E0730=0;
        definition.filter.categoryA4=*s.world.physicsBodyCategory3E0718;
        s.components[1]=&component;s.e4ConstructorReached=true;return true;
    }
    bool loadFullE4Type0(const std::shared_ptr<rr::assets::OriginalStandardTextureResolver50LA>& resolver,
        rr::assets::OriginalStandardTextureDiagnostic50LA& diagnostic)noexcept {
        if(!storage_||!storage_->e4ConstructorReached||storage_->e4ConstructorRetired||
           storage_->published[1]||storage_->queries[1].type0Owners.attempted())return false;
        //125004 borrows the SAME canonical model already published by E0.
        return storage_->queries[1].type0Owners.load11B920(*storage_->assets,
            SpringSceneId48N{modelSceneIdentity().value},resolver,diagnostic);
    }
    OriginalWreckQueryResult50LA preparePublishFullE411F570()noexcept {
        using S=OriginalWreckQueryStatus50LA;
        if(!storage_||!storage_->e4ConstructorReached||storage_->e4ConstructorRetired||
           storage_->e4CallbacksBound||storage_->published[1]||storage_->published[2]||
           storage_->initialShapesReady||!storage_->contactBound||!modelSceneIdentity().value)
            return {S::binding,0x1662a0u};
        auto& s=*storage_;auto& q=s.queries[1];const auto* resource=q.type0Owners.resource();
        if(!q.type0Owners.loaded()||!resource||!resource->live()||!s.components[1]||
           s.components[1]->reference00!=1u||q.type0.registrationE0||
           q.type0Owners.shape8C_->model110.value!=modelSceneIdentity().value||
           resource->root1C()>=resource->mesh().nodes.size()||!s.piece->coherent()||
           !canRetireE4Prefix11B4A0(*s.components[1]))return {S::binding,0x11f570u};
        //1662A8/B0/BC are actual source stores before the model refresh.
        q.responseE8=ResponseCallback::physics165920;q.owner9C=s.piece;q.bodyA0=&s.physical->body.root;
        s.e4CallbacksBound=true;
        auto graph=s.assets->graph();const auto model=s.assets->model14C();
        if(!refreshSceneWorld49P(graph,model))return {S::graph,0x28b550u};
        const auto& world=graph.nodes[static_cast<std::int32_t>(model)].world100;
        const auto& mesh=resource->mesh();const auto& root=mesh.nodes[resource->root1C()];
        std::memcpy(q.fullType0Source.local40.v,mesh.local_matrix.data(),sizeof(BodyMatrix48G));
        std::memcpy(q.fullType0Source.rootCenter.v,root.center.data(),sizeof(Vector3_48C));
        std::memcpy(q.fullType0Source.rootHalf.v,root.half.data(),sizeof(Vector3_48C));
        //11F570 calls11ED60, NOT ordinary11FE50.11EDB4..11EE04
        //initialize BOTH histories before11EF24 calls11F680 with SAME world.
        q.type0.previousC0=world;q.type0.world80=world;
        if(!preparePodShape50IZ(q.fullType0Source,world,q.type0))return {S::preparation,0x11f680u};
        s.e4Prepared=true;auto& cold=s.coldStates[1];cold.cachedCenter=q.type0.center60;cold.cachedRadius=q.type0.radius80;
        //Native canonical publication follows actual completed shape setup.
        //No scene-domain identity is reminted;110 borrows E0's existing model.
        const auto index=s.world.refs[1]-1u;auto definition=s.coldDefinitions[1];
        definition.hasShape=true;definition.shapeTag=0;
        s.world.objects[index]=definition;s.world.states[index]=cold;
        s.world.copies[index].copied40=q.constructorCopied40;s.world.copies[index].copied50=q.constructorCopied50;
        s.world.geometry[index]={&mesh,q.type0.composed00,nullptr,true};
        s.published[1]=true;s.e4FullPublication=true;return {S::complete,0x1662dcu,true,false};
    }
    bool fullCategoryOwnership()const noexcept {
        if(!storage_||!storage_->fullCollectionAttempted)return false;
        const auto& s=*storage_;if(s.fullCollectionCursor>2u||!s.fullCollectionWater)return false;
        for(unsigned i=0;i<2;++i){const auto& owner=s.sourceCategoryD8[i];
            const auto& view=object(i?2u:0u).filter.categories;
            const auto n=i?(1u+((s.fullCollectionCursor==2u&&s.fullCollectionWater!=*s.world.physicsBodyCategory3E0718)?1u:0u)):
                (s.fullCollectionCursor?1u:0u);
            if(owner.extent()!=n)return false;
            if(owner.retired()){
                if((i?!s.e8ConstructorRetired:!s.e0ConstructorRetired)||view.values||
                   view.capacity!=n||view.count!=static_cast<std::int32_t>(n)||view.mode!=1)return false;
            }else{
                if(!owner.matches(view))return false;
                if(n&&view.values[0]!=(i?*s.world.physicsBodyCategory3E0718:s.fullCollectionWater))return false;
                if(n==2u&&view.values[1]!=s.fullCollectionWater)return false;
            }
        }
        return true;
    }
    OriginalWreckQueryResult50LA constructFullCollectionFilters22C21C(const std::uint32_t& actualWater4342A8)noexcept {
        using S=OriginalWreckQueryStatus50LA;
        if(!storage_||storage_->fullCollectionAttempted||storage_->collectionFilterApplied||!actualWater4342A8||
           !storage_->e0FullPublication||!storage_->e8FullPublication||storage_->e0ConstructorRetired||
           storage_->e8ConstructorRetired||!storage_->derivedBound||!fullMutualOwnership()||
           !storage_->components[2]||!canRetireFullE811B4A0(*storage_->components[2])||
           !storage_->sourceCategoryD8[0].matches(object(0).filter.categories))return {S::binding,0x22c21cu};
        auto& s=*storage_;s.fullCollectionAttempted=true;s.fullCollectionWater=actualWater4342A8;
        object(0).filter.categories.mode=1; //124160,22C21C
        if(!s.sourceCategoryD8[0].append124170(object(0).filter.categories,actualWater4342A8))return {S::allocation,0x22c238u};
        s.fullCollectionCursor=1;
        if(!s.sourceCategoryD8[1].append124170(object(2).filter.categories,actualWater4342A8))return {S::allocation,0x22c254u};
        s.fullCollectionCursor=2;s.collectionFilterApplied=true;
        return {S::complete,0x22c25cu};
    }
    bool appendNativeOwnPod694123EB0(std::uint32_t actualPod694,std::uint32_t& actualE4)noexcept {
        actualE4=0;
        if(!storage_||storage_->nativeRacerPodAttempted||!actualPod694||
           !storage_->collectionFilterApplied||!fullCategoryOwnership()||!fullMutualOwnership()||
           !storage_->e0FullPublication||!storage_->e4FullPublication||storage_->e0ConstructorRetired||
           storage_->e4ConstructorRetired||!storage_->components[0]||!storage_->components[1]||
           storage_->components[0]->reference00!=1u||storage_->components[1]->reference00!=1u)return false;
        auto& s=*storage_;for(const auto id:s.world.identities)if(id==actualPod694)return false;
        s.nativeRacerPodAttempted=true;s.nativeRacerPod694Identity=actualPod694;
        if(!s.sourceIdentityC0[0].appendNativeRacer123EB0(object(0).filter.identities[0],actualPod694))return false;
        s.nativeRacerPodCommitted=true;actualE4=s.world.identities[1];return true;
    }
    bool fullMutualOwnership()const noexcept {
        if(!storage_||!storage_->fullMutualAttempted)return false;
        const auto& s=*storage_;if(s.fullMutualCursor>4u)return false;
        for(std::size_t i=0;i<2;++i){const auto& owned=s.sourceIdentityC0[i];const auto& view=object(i).filter.identities[0];
            const auto n=i==0?(s.fullMutualCursor<2u?s.fullMutualCursor:2u)+(s.nativeRacerPodCommitted?1u:0u):
                (s.fullMutualCursor>2u?s.fullMutualCursor-2u:0u);
            if(owned.extent()!=n)return false;
            if(owned.retired()){
                if((i==0?!s.e0ConstructorRetired:!s.e4ConstructorRetired)||view.values||view.capacity!=n||view.mode!=1||view.count!=static_cast<std::int32_t>(n))return false;
            }else{
                if(!owned.matches(view))return false;
                if(n&&view.values[0]!=s.world.identities[i==0?1u:0u])return false;
                if(n>=2u&&view.values[1]!=s.world.identities[2])return false;
                if(n==3u&&(!s.nativeRacerPodAttempted||!s.nativeRacerPod694Identity||
                   view.values[2]!=s.nativeRacerPod694Identity))return false;
            }
        }
        return true;
    }
    OriginalWreckQueryResult50LA constructFullMutual166390()noexcept {
        using S=OriginalWreckQueryStatus50LA;
        if(!storage_||storage_->fullMutualAttempted||storage_->mutualBound||storage_->derivedBound||
           !storage_->e8FullPublication||!storage_->e4FullPublication||!storage_->e0FullPublication||
           storage_->initialShapesReady||!storage_->components[2]||
           !canRetireFullE811B4A0(*storage_->components[2]))return {S::binding,0x166390u};
        auto& s=*storage_;
        for(std::size_t i=0;i<2;++i){
            if(!s.components[i]||s.components[i]->reference00!=1u||!s.published[i]||
               !s.sourceIdentityC0[i].matches(object(i).filter.identities[0]))return {S::binding,0x166394u};
        }
        s.fullMutualAttempted=true;
        object(0).filter.identities[0].mode=1; //166394:123EA0(E0,1)
        object(1).filter.identities[0].mode=1; //1663A0:123EA0(E4,1)
        const std::size_t owner[4]={0,0,1,1},target[4]={1,2,0,2};
        const std::uint32_t pc[4]={0x1663acu,0x1663b8u,0x1663c4u,0x1663d0u};
        for(std::size_t i=0;i<4;++i){
            if(!s.sourceIdentityC0[owner[i]].append123EB0(object(static_cast<unsigned>(owner[i])).filter.identities[0],
                s.world.identities[target[i]]))return {S::allocation,pc[i]};
            ++s.fullMutualCursor;
        }
        s.mutualBound=true;return {S::complete,0x1663d8u};
    }
    bool bindFullDerived2CB610()noexcept {
        if(!storage_||!storage_->mutualBound||storage_->derivedBound||storage_->fullMutualCursor!=4u||
           !fullMutualOwnership()||!storage_->published[1]||storage_->e4ConstructorRetired||
           query(1).responseE8!=ResponseCallback::physics165920)return false;
        query(1).responseE8=ResponseCallback::wreck2CB420;storage_->derivedBound=true;return true;
    }
    bool canConstructE8Prefix11B190()const noexcept {
        if(!storage_||!storage_->e4FullPublication||storage_->e4ConstructorRetired||storage_->e8ConstructorReached||
           storage_->components[2]||storage_->published[2]||storage_->initialShapesReady)return false;
        const auto& s=*storage_;const auto& q=s.queries[2];const auto& r=s.world.lifecycle->storage.registrations[s.world.refs[2]-1u];
        return !r.owner&&!r.slot&&!r.encodedObject&&!q.membership.attached&&!q.membership.object&&!q.membership.revision;
    }
    bool constructE8Prefix11B190(OriginalWreckComponent50LA& component,
        std::optional<OriginalWreckComponent50LA*>* nativePublication=nullptr)noexcept {
        if(nativePublication&&nativePublication->has_value())return false;
        if(!canConstructE8Prefix11B190()||storage_->plannedQueries[2]!=&component||component.reference00!=1u||
           component.kind!=OriginalWreckComponentKind50LA::collision11B190)return false;
        auto& s=*storage_;auto& q=s.queries[2];auto& d=s.coldDefinitions[2];auto& state=s.coldStates[2];
        d.hasShape=false;d.shapeTag=0;d.filter.flags31=component.flags29;d.filter.categoryA4=0;
        for(auto& list:d.filter.identities)list={nullptr,0,0,1};d.filter.categories={nullptr,0,0,1};
        q.owner9C=nullptr;q.contactOwner9C=nullptr;q.bodyA0=nullptr;q.queryE4=QueryCallback::none;q.responseE8=ResponseCallback::none;
        q.constructorCopied40=*s.world.sentinel3E02C0;q.constructorCopied50=*s.world.sentinel3E02C0;
        q.previousCenter70={{0.f,-10000.f,0.f,1.f}};q.distance84=0.f;q.currentIdentity94=0;
        state.flagsF8=static_cast<std::uint8_t>((state.flagsF8&0xc0u)|6u);
        if(nativePublication)*nativePublication=&component; //166300 AFTER actual11B190 defaults.
        state.flagsF8=static_cast<std::uint8_t>((state.flagsF8&~0x0eu)|2u); //11B570(3)
        *s.world.globalValid3E0730=0;s.components[2]=&component;s.e8ConstructorReached=true;return true;
    }
    OriginalWreckQueryResult50LA loadPublishFullE811C710()noexcept {
        using S=OriginalWreckQueryStatus50LA;
        if(!storage_||!storage_->e8ConstructorReached||storage_->e8ConstructorRetired||storage_->e8OwnerBound||
           storage_->published[2]||storage_->initialShapesReady||!storage_->components[2])return {S::binding,0x16633cu};
        auto& s=*storage_;auto& q=s.queries[2];
        q.owner9C=s.piece;s.coldDefinitions[2].filter.categoryA4=*s.world.physicsBodyCategory3E0718;s.e8OwnerBound=true;
        if(!q.type3Owners.allocate11C710(*s.world.sentinel3E02C0))return {S::allocation,q.type3Owners.sourceBoundary()};
        //Source11C710 has initialized vectors and scalar counts, NOT radius,
        //lengths or selected hit state. Retain cold cache scalar words.
        const auto& shape=*q.type3Owners.shape8C_;const auto& result=*q.type3Owners.result90_;
        std::memcpy(q.type3.localCenter.v,shape.center20.v,sizeof(Vector3_48C));
        q.type3.worldCenter=shape.center30;q.type3.queryCenter=s.coldStates[2].cachedCenter;
        q.type3.result00=result.first00;q.type3.result10=result.second10;
        q.type3.selectedNormal=result.normal20;q.type3.selectedPoint=result.point30;q.type3.selectedCorrection=result.correction40;
        std::memcpy(q.ray.localStart.v,shape.localStarts0C[0].v,sizeof(Vector3_48C));
        std::memcpy(q.ray.localDirection.v,shape.localDirections10[0].v,sizeof(Vector3_48C));
        q.ray.worldStart=shape.worldStarts18[0];q.ray.worldDirection=shape.worldDirections1C[0];
        q.ray.normal=result.normals54[0];q.ray.point=result.points58[0];
        q.normalMode=shape.normalMode44;
        const auto index=s.world.refs[2]-1u;auto d=s.coldDefinitions[2];d.hasShape=true;d.shapeTag=3;
        s.world.objects[index]=d;s.world.states[index]=s.coldStates[2];
        s.world.copies[index].copied40=q.constructorCopied40;s.world.copies[index].copied50=q.constructorCopied50;
        s.world.geometry[index]={};s.published[2]=true;s.e8FullPublication=true;
        object(2).filter.categories.mode=1; //124160(1), repeats constructor D4=1
        if(!s.sourceCategoryD8[1].append124170(object(2).filter.categories,*s.world.physicsBodyCategory3E0718))
            return {S::allocation,0x124214u};
        state(2).flagsF8=static_cast<std::uint8_t>((state(2).flagsF8&~1u)|1u); //16638C
        return {S::complete,0x166390u};
    }
    bool canRetireFullE811B4A0(const OriginalWreckComponent50LA& component)const noexcept {
        if(!storage_||!storage_->e8ConstructorReached||storage_->e8ConstructorRetired||storage_->components[2]!=&component||
           component.reference00!=1u||component.firstChild18||component.kind!=OriginalWreckComponentKind50LA::collision11B190||
           storage_->initialShapesReady||((storage_->mutualBound||storage_->derivedBound)&&!storage_->fullMutualAttempted)||(storage_->collectionFilterApplied&&!storage_->fullCollectionAttempted)||
           (storage_->fullCollectionAttempted&&!fullCategoryOwnership())||
           (storage_->fullMutualAttempted&&!fullMutualOwnership()))return false;
        const auto& s=*storage_;const auto& q=s.queries[2];const auto& r=s.world.lifecycle->storage.registrations[s.world.refs[2]-1u];
        if(r.owner||r.slot||r.encodedObject||q.membership.attached||q.membership.object||q.membership.revision||
           q.owner9C!=(s.e8OwnerBound?s.piece:nullptr)||q.contactOwner9C||q.bodyA0||
           q.queryE4!=QueryCallback::none||q.responseE8!=ResponseCallback::none||
           (q.type3Owners.attempted()&&!q.type3Owners.canRelease11B600()))return false;
        for(std::size_t i=0;i<s.capacity;++i){const auto& e=q.entries[i];if(e.used||e.registration.owner||e.registration.slot||e.registration.encodedObject)return false;}
        if(s.published[2]!=s.e8FullPublication)return false;
        if(s.e8FullPublication){const auto& o=object(2);const auto& c=o.filter.categories;
            if(o.identity!=s.world.identities[2]||!o.hasShape||o.shapeTag!=3u||o.group||
               o.filter.flags31!=component.flags29||!s.sourceCategoryD8[1].matches(c))return false;
            for(const auto& list:o.filter.identities)if(list.values||list.count)return false;
        }
        return true;
    }
    bool retireFullE811B4A0(OriginalWreckComponent50LA& component,std::uint32_t& count)noexcept {
        if(!storage_||!storage_->e8ConstructorReached||storage_->e8ConstructorRetired||
           storage_->components[2]!=&component||component.reference00)return false;
        auto& s=*storage_;auto& q=s.queries[2];
        //11B51C, AFTER genuineNULL C0/CC, BEFORE11B600. Retain DC/mode.
        if(s.e8FullPublication&&!s.sourceCategoryD8[1].release11B51C(object(2).filter.categories))return false;
        if(q.type3Owners.attempted()&&!q.type3Owners.release11B600())return false;
        if(s.e8FullPublication)object(2).hasShape=false;
        component.kind=OriginalWreckComponentKind50LA::entity141300;--count;
        q.owner9C=nullptr;s.components[2]=nullptr;s.published[2]=false;s.e8ConstructorRetired=true;return true;
    }
    bool canRetireFullE411B4A0(const OriginalWreckComponent50LA& component)const noexcept {
        if(storage_&&storage_->e8ConstructorReached&&!storage_->e8ConstructorRetired)return false;
        if(storage_&&!storage_->e4CallbacksBound)return canRetireE4Prefix11B4A0(component);
        if(!storage_||!storage_->e4CallbacksBound||storage_->e4ConstructorRetired||
           storage_->components[1]!=&component||component.reference00!=1u||component.firstChild18||
           component.kind!=OriginalWreckComponentKind50LA::collision11B190||storage_->published[2]||
           storage_->initialShapesReady||((storage_->mutualBound||storage_->derivedBound)&&!storage_->fullMutualAttempted)||(storage_->collectionFilterApplied&&!storage_->fullCollectionAttempted)||
           (storage_->fullCollectionAttempted&&!fullCategoryOwnership())||
           (storage_->fullMutualAttempted&&!fullMutualOwnership()))return false;
        const auto& s=*storage_;const auto& q=s.queries[1];const auto* resource=q.type0Owners.resource();
        if(!q.type0Owners.loaded()||!resource||!resource->live()||!modelSceneIdentity().value||
           q.type0Owners.shape8C_->model110.value!=modelSceneIdentity().value||q.type0.registrationE0||
           q.membership.attached||q.membership.object||q.membership.revision||q.owner9C!=s.piece||
           q.contactOwner9C||q.bodyA0!=&s.physical->body.root||q.queryE4!=QueryCallback::none||
           q.responseE8!=(s.derivedBound?ResponseCallback::wreck2CB420:ResponseCallback::physics165920))return false;
        const auto& r=s.world.lifecycle->storage.registrations[s.world.refs[1]-1u];
        if(r.owner||r.slot||r.encodedObject)return false;
        for(std::size_t i=0;i<s.capacity;++i){const auto& e=q.entries[i];
            if(e.used||e.registration.owner||e.registration.slot||e.registration.encodedObject)return false;}
        if(s.published[1]!=s.e4FullPublication)return false;
        if(s.e4FullPublication){
            const auto& o=object(1);const auto& g=s.world.geometry[s.world.refs[1]-1u];
            if(!s.e4Prepared||o.identity!=s.world.identities[1]||!o.hasShape||o.shapeTag||o.group||
               o.typeTag!=*s.world.collisionTypeGp8010||o.filter.flags31!=component.flags29||
               g.mesh!=&resource->mesh()||g.compound||!g.hasResponseCallbackE8)return false;
            if(s.fullMutualAttempted){if(!s.sourceIdentityC0[1].matches(o.filter.identities[0]))return false;}
            else if(o.filter.identities[0].values||o.filter.identities[0].count)return false;
            if(o.filter.identities[1].values||o.filter.identities[1].count)return false;
            if(o.filter.categories.values||o.filter.categories.count)return false;
        }
        return true;
    }
    bool retireFullE411B4A0(OriginalWreckComponent50LA& component,std::uint32_t& count)noexcept {
        if(storage_&&!storage_->e4CallbacksBound)return retireE4Prefix11B4A0(component,count);
        if(!storage_||!storage_->e4CallbacksBound||storage_->components[1]!=&component||
           component.reference00||storage_->e4ConstructorRetired)return false;
        auto& s=*storage_;auto& q=s.queries[1];
        if(s.fullMutualAttempted&&!s.sourceIdentityC0[1].release11B4F4(object(1).filter.identities[0]))return false;
        if(!q.type0Owners.release11B600())return false;
        if(s.e4FullPublication){
            object(1).hasShape=false; //11B900; preserve shape tag/filter/bounds/history
            auto& geometry=s.world.geometry[s.world.refs[1]-1u];
            geometry.mesh=nullptr;geometry.hasResponseCallbackE8=false;
            //Only now has the actual shape died. End its native geometry alias;
            //do not erase the canonical row or reset its world/history matrix.
        }
        component.kind=OriginalWreckComponentKind50LA::entity141300;--count;
        q.owner9C=nullptr;q.bodyA0=nullptr; //native borrows end after actual lifetime
        s.components[1]=nullptr;s.published[1]=false;s.e4ConstructorRetired=true;s.derivedBound=false;return true;
    }
    bool canRetireE4Prefix11B4A0(const OriginalWreckComponent50LA& component)const noexcept {
        if(!storage_||!storage_->e4ConstructorReached||storage_->e4ConstructorRetired||
           storage_->e4CallbacksBound||storage_->components[1]!=&component||component.reference00!=1u||component.parent1C||
           component.previous10||component.next14||component.firstChild18||
           component.kind!=OriginalWreckComponentKind50LA::collision11B190||storage_->published[1]||
           storage_->published[2]||storage_->initialShapesReady)return false;
        const auto& s=*storage_;const auto& q=s.queries[1];const auto& r=s.world.lifecycle->storage.registrations[s.world.refs[1]-1u];
        if(r.owner||r.slot||r.encodedObject||q.membership.attached||q.membership.object||q.membership.revision||
           q.owner9C||q.contactOwner9C||q.bodyA0||q.queryE4!=QueryCallback::none||q.responseE8!=ResponseCallback::none)return false;
        for(std::size_t i=0;i<s.capacity;++i){const auto& e=q.entries[i];
            if(e.used||e.registration.owner||e.registration.slot||e.registration.encodedObject)return false;}
        return true;
    }
    bool retireE4Prefix11B4A0(OriginalWreckComponent50LA& component,std::uint32_t& count)noexcept {
        if(!storage_||storage_->components[1]!=&component||component.reference00||storage_->e4ConstructorRetired||
           !storage_->e4ConstructorReached||storage_->e4CallbacksBound||storage_->published[1])return false;
        auto& owned=storage_->queries[1].type0Owners;
        if(owned.attempted()&&!owned.release11B600())return false;
        component.kind=OriginalWreckComponentKind50LA::entity141300;--count;
        storage_->components[1]=nullptr;storage_->e4ConstructorRetired=true;return true;
    }
    bool canRetireFullContactSuffix()const noexcept {
        if(!storage_||!storage_->e0FullPublication||storage_->e0ConstructorRetired||
           (storage_->e4ConstructorReached&&!storage_->e4ConstructorRetired)||
           storage_->initialShapesReady||storage_->published[1]||storage_->published[2]||
           (storage_->mutualBound&&!storage_->fullMutualAttempted)||storage_->derivedBound||(storage_->collectionFilterApplied&&!storage_->fullCollectionAttempted)||
           (storage_->fullCollectionAttempted&&!fullCategoryOwnership())||
           (storage_->fullMutualAttempted&&!fullMutualOwnership())||!modelSceneIdentity().value)return false;
        const auto& s=*storage_;const auto& q=s.queries[0];
        if(!q.type2||!q.type2->sourceOwners.loaded()||q.membership.attached||
           q.membership.object||q.membership.revision)return false;
        const auto& r=s.world.lifecycle->storage.registrations[s.world.refs[0]-1u];
        if(r.owner||r.slot||r.encodedObject)return false;
        for(std::size_t i=0;i<s.capacity;++i){const auto& e=q.entries[i];
            if(e.used||e.registration.owner||e.registration.slot||e.registration.encodedObject)return false;}
        return true;
    }
    bool bindFullContact2955F0(const OriginalWreckComponent50LA& contact,
        const OriginalWreckComponent50LA& detector)noexcept {
        if(!storage_||!storage_->e0FullPublication||!storage_->e0Prepared||storage_->contactBound||
           storage_->e0ConstructorRetired||!storage_->physical->contactConstructorFieldsApplied||
           storage_->physical->contactBody30!=&storage_->physical->body||
           contact.kind!=OriginalWreckComponentKind50LA::contact295A40||contact.reference00!=1u||
           detector.kind!=OriginalWreckComponentKind50LA::detector295290||detector.reference00!=1u||
           contact.sessionGeneration!=storage_->piece->sessionGeneration()||
           detector.sessionGeneration!=storage_->piece->sessionGeneration())return false;
        auto& q=storage_->queries[0];q.owner9C=nullptr;q.contactOwner9C=storage_->physical;
        q.bodyA0=&storage_->physical->body.root;q.queryE4=QueryCallback::contact294BB0;
        object(0).filter.identities[0].mode=1; //123EA0 writes sourceBC
        storage_->fullContactComponent=&contact;storage_->fullDetectorComponent=&detector;
        storage_->contactBound=true;return true;
    }
    bool canRetireFullE011B4A0(const OriginalWreckComponent50LA& component)const noexcept {
        if(!storage_||!storage_->e0FullPublication||storage_->e0ConstructorRetired||
           (storage_->e4ConstructorReached&&!storage_->e4ConstructorRetired)||
           storage_->components[0]!=&component||component.reference00!=1u||component.firstChild18||
           storage_->published[1]||storage_->published[2]||(storage_->mutualBound&&!storage_->fullMutualAttempted)||
           storage_->derivedBound||(storage_->collectionFilterApplied&&!storage_->fullCollectionAttempted)||
           (storage_->fullCollectionAttempted&&!fullCategoryOwnership())||
           (storage_->fullMutualAttempted&&!fullMutualOwnership()))return false;
        const auto& s=*storage_;const auto& q=s.queries[0];const auto& w=s.world;
        if(!q.type2||!q.type2->sourceOwners.loaded()||!s.sceneDomain->canRetireShape11B600(s.modelScene44,component)||
           q.membership.attached||q.membership.object||q.membership.revision||
           q.responseE8!=ResponseCallback::none||q.bodyA0!=&s.physical->body.root)return false;
        if(s.contactBound){
            // D8/DC died first, in actual tail order. Their source callbacks
            // were not cleared: do not consume them as live receiver borrows.
            if(!s.fullContactComponent||!s.fullDetectorComponent||
               s.fullContactComponent->reference00||s.fullDetectorComponent->reference00||
               s.fullContactComponent->kind!=OriginalWreckComponentKind50LA::entity141300||
               s.fullDetectorComponent->kind!=OriginalWreckComponentKind50LA::entity141300||
               q.owner9C||q.contactOwner9C!=s.physical||q.queryE4!=QueryCallback::contact294BB0||
               s.physical->records.size()||s.physical->pointRecords.size())return false;
        }else if(q.owner9C!=s.piece||q.contactOwner9C||q.queryE4!=QueryCallback::none)return false;
        const auto& registration=w.lifecycle->storage.registrations[w.refs[0]-1u];
        if(registration.owner||registration.slot||registration.encodedObject)return false;
        for(std::size_t i=0;i<s.capacity;++i){const auto& e=q.entries[i];
            if(e.used||e.registration.owner||e.registration.slot||e.registration.encodedObject)return false;}
        if(object(0).typeTag!=*w.collisionTypeGp8010||object(0).group||
           object(0).filter.flags31!=component.flags29)return false;
        const auto& f=object(0).filter;
        if(s.fullMutualAttempted){if(!s.sourceIdentityC0[0].matches(f.identities[0]))return false;}
        else if(f.identities[0].values||f.identities[0].count)return false;
        if(f.identities[1].values||f.identities[1].count)return false;
        return s.fullCollectionAttempted?s.sourceCategoryD8[0].matches(f.categories):
            (!f.categories.values&&!f.categories.count);
    }
    bool retireFullE011B4A0(OriginalWreckComponent50LA& component,std::uint32_t& objectCount)noexcept {
        // Caller already admitted canRetireFullE0 and performed142590/104970.
        if(!storage_||component.reference00||storage_->components[0]!=&component||
           !storage_->e0FullPublication||storage_->e0ConstructorRetired)return false;
        auto& s=*storage_;auto& q=s.queries[0];
        if(s.fullMutualAttempted&&!s.sourceIdentityC0[0].release11B4F4(object(0).filter.identities[0]))return false;
        if(s.fullCollectionAttempted&&!s.sourceCategoryD8[0].release11B51C(object(0).filter.categories))return false;
        if(!q.type2->sourceOwners.releaseShapeResult11B600())return false;
        object(0).hasShape=false; //11B900 clears8C, leaves source88/category/history
        if(!s.sceneDomain->retireShape11B600(s.modelScene44,q.type2->sourceOwners,component))std::terminate();
        component.kind=OriginalWreckComponentKind50LA::entity141300;--objectCount;
        // End native borrows only AFTER actual E0 lifetime; these are not
        // claimed source stores clearing the dead object's callback words.
        q.owner9C=nullptr;q.bodyA0=nullptr;q.contactOwner9C=nullptr;q.queryE4=QueryCallback::none;
        s.contactBound=false;s.mutualBound=false;s.components[0]=nullptr;
        s.published[0]=false;s.e0ConstructorRetired=true;return true;
    }
    bool canRetireE0Prefix11B4A0(const OriginalWreckComponent50LA& component)const noexcept {
        if(!storage_||!storage_->e0ConstructorReached||storage_->e0ConstructorRetired||
           storage_->components[0]!=&component||storage_->published[0]||storage_->published[1]||storage_->published[2]||
           storage_->initialShapesReady||storage_->initialShapesAttempted||storage_->contactBound||
           storage_->mutualBound||storage_->derivedBound||(storage_->collectionFilterApplied&&!storage_->fullCollectionAttempted)||
           (storage_->fullCollectionAttempted&&!fullCategoryOwnership())||
           (storage_->queries[0].type2&&storage_->queries[0].type2->sourceOwners.attempted()))return false;
        const auto& w=storage_->world;const auto& q=storage_->queries[0];
        const auto& r=w.lifecycle->storage.registrations[w.refs[0]-1u];
        return !r.owner&&!r.slot&&!r.encodedObject&&!q.membership.attached&&!q.membership.object&&
            !q.membership.revision&&!storage_->coldDefinitions[0].hasShape&&
            storage_->coldDefinitions[0].shapeTag==0u&&component.kind==OriginalWreckComponentKind50LA::collision11B190;
    }
    bool retireE0Prefix11B4A0(OriginalWreckComponent50LA& component,std::uint32_t& actualObjectCount423F60)noexcept {
        if(!canRetireE0Prefix11B4A0(component)||component.reference00||component.parent1C||
           component.firstChild18||component.previous10||component.next14)return false;
        //Actual11B190 initialized registrationE0,filtersC0/CC/D8,shape8C,
        //result90 NULL. No11BBC0/11F570/registration call occurred, so11B4A0
        //and11B600 take their genuine null-owner branches before1413E0.
        component.kind=OriginalWreckComponentKind50LA::entity141300;--actualObjectCount423F60;
        auto& s=*storage_;s.queries[0].owner9C=nullptr;s.queries[0].bodyA0=nullptr;
        s.components[0]=nullptr;s.e0ConstructorRetired=true;return true;
    }
    static OriginalWreckQueryResult50LA stageInitialModelShapes(Storage& storage)noexcept {
        using S=OriginalWreckQueryStatus50LA;
        auto& assets=*storage.assets;auto graph=assets.graph();const auto model=assets.model14C();
        if(!refreshSceneWorld49P(graph,model))return {S::graph,0x28b550u};
        const auto& world=graph.nodes[static_cast<std::int32_t>(model)].world100;
        QuerySpherePrepareOutput48O sphere{};
        if(!prepareType2(assets.queryE0(),world,sphere))return {S::preparation,0x11f680u};
        storage.queries[0].type2World80=sphere.world;
        storage.coldStates[0].cachedCenter=sphere.center;storage.coldStates[0].cachedRadius=sphere.radius;
        auto& shape=storage.queries[1].type0;shape.world80=world;shape.previousC0=world;
        if(!preparePodShape50IZ(assets.targetE4().source,world,shape))return {S::preparation,0x11f680u};
        storage.coldStates[1].cachedCenter=shape.center60;storage.coldStates[1].cachedRadius=shape.radius80;
        storage.initialShapesReady=true;return {S::complete,0};
    }
    std::unique_ptr<Storage> storage_;
    bool retired_=false; // terminal cancellation cannot prepare/rebind again
    static bool prepareType2(const Type2ResourceView49X& resource,const BodyMatrix48G& world,
        QuerySpherePrepareOutput48O& out) noexcept {
        QuerySpherePrepareInput48O in{};in.preparedScene=world;in.shapeType=2;
        if(resource.root&type2Leaf49X){
            const auto ray=resource.root&~type2Leaf49X;
            if(ray>=resource.rayCount)return false;
            in.first=resource.rays[ray].first;in.second=resource.rays[ray].second;
            in.pairCount=1;in.hasPairTree=false;
        }else{
            if(resource.root>=resource.nodeCount)return false;
            in.hasPairTree=true;const auto& root=resource.nodes[resource.root];
            for(unsigned axis=0;axis<3;++axis){in.treeCenter.v[axis]=root.center[axis];in.treeHalf.v[axis]=root.half[axis];}
        }
        return prepareQuerySphere48O(in,out)==QuerySpherePrepareStatus48O::Ready;
    }
    LiveZoneBounds50EN bounds(unsigned i)const noexcept {
        const auto& q=query(i);const auto& cache=state(i);LiveZoneBounds50EN b{};
        b.bounds.shapeTag=i==0?2u:i==1?0u:3u;
        b.bounds.cachedCenter=cache.cachedCenter;b.bounds.cachedRadius=cache.cachedRadius;
        if(i==0){b.bounds.shapeWorld=q.type2World80;b.type2Resource=&storage_->assets->queryE0();}
        else if(i==1){b.bounds.shapeWorld=q.type0.composed00;b.type0Resource=storage_->e4FullPublication?&q.fullType0Source:&storage_->assets->targetE4().source;}
        else {b.bounds.pairCount=1;b.bounds.endpoints=q.movementEndpoints();b.bounds.endpointCount=2;
            b.bounds.endpointScenes=q.movementMetadata();b.bounds.endpointSceneCount=2;}
        return b;
    }
    void publishBounds(unsigned i) noexcept {
        auto& cache=state(i);const auto& membership=query(i).membership;
        cache.cachedCenter=membership.bounds.cachedCenter;cache.cachedRadius=membership.bounds.cachedRadius;
        // No F8, iterator, selected hit or collision40/50 state is overwritten.
    }
};
} // namespace rr::game

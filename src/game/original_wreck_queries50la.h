#pragma once
#include "original_wreck_assets50la.h"
#include "original_wreck_physical50la.h"
#include "body_wrapper48h.h"
#include "live_zone_membership50en.h"
#include "query_prepare48o.h"
#include "type3_state48p.h"
#include "original_camera_world_query.h"
#include "collision_normal_mode50ja.h"
#include "original_wreck_type2_storage50la.h"
#include <array>
#include <algorithm>
#include <cstring>
#include <limits>
#include <memory>

namespace rr::game {
class OriginalOnePieceWreckOwner50LA;

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
public:
    enum class QueryCallback { none,contact294BB0 };
    enum class ResponseCallback { none,physics165920,wreck2CB420 };
    struct Query {
        // One source9C slot with two typed alternatives.2955F0 overwrites
        // E0's initial piece owner with the actual DC contact owner and
        // installs294BB0 in E4. E4/E8 retain their piece owner9C.
        OriginalOnePieceWreckOwner50LA* owner9C=nullptr;
        OriginalWreckPhysical50LA* contactOwner9C=nullptr;
        QueryCallback queryE4=QueryCallback::none;
        RootBodyState48H* bodyA0=nullptr; // E8 source ctor leaves this null
        ResponseCallback responseE8=ResponseCallback::none;
        SpringVector4_48N previousCenter70{};
        float distance84=0.f;
        std::uint32_t currentIdentity94=0;
        std::unique_ptr<CollectionZoneLink50BX[]> links;
        std::unique_ptr<LiveZoneEntry50EN[]> entries;
        LiveZoneMembership50EN membership;
        // E0 stores world80; E4 stores the complete actual shape0 state.
        BodyMatrix48G type2World80{};
        PodShapeState49W type0{};
        // E8's exact one-pair shape. Its source11FF60 preparation is a no-op;
        //1202E0 still recomputes the registration sphere from these endpoints.
        SpringVector4_48N endpoints[2]{};
        std::uint32_t endpointMetadata[2]{};
        Type3QueryState48P type3{};
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
    OriginalWreckQueryResult50LA prepareCold(OriginalWreckAssets50LA& assets,
        OriginalOnePieceWreckOwner50LA& piece,OriginalWreckPhysical50LA& physical,
        const OriginalWreckQueryWorld50LA& world) noexcept {
        using S=OriginalWreckQueryStatus50LA;
        if(storage_||!assets.prepared()||!world.lifecycle||!world.world||!world.workspace||
           !world.objects||!world.states||!world.copies||!world.geometry||!world.objectCount||
           world.objectCount!=world.lifecycle->objectCount||!world.physicsBodyCategory3E0718||
           !*world.physicsBodyCategory3E0718||!world.collisionTypeGp8010||
           *world.collisionTypeGp8010==0xffu||!world.sentinel3E02C0||!world.globalValid3E0730||
           !world.world->zoneCount||!world.lifecycle->storage.registrations||
           world.lifecycle->storage.registrationCapacity<world.objectCount||
           world.world->portalCount==std::numeric_limits<std::size_t>::max())
            return {S::binding,0x165e90u};
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
            next->piece=&piece;next->physical=&physical;
            next->capacity=world.world->portalCount+1u;
            const auto category=*world.physicsBodyCategory3E0718;
            next->exclusions[0]={world.identities[1],world.identities[2]};
            next->exclusions[1]={world.identities[0],world.identities[2]};
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
            //165E90 calls11F570 for E0 and E4 immediately after loading.
            //11ED60 seeds both type0 world histories from the ACTUAL model
            //world, then11F680 computes the first shape/sphere. These are not
            //zero or identity histories. The reached getter's graph refresh
            //survives a later cold rejection, matching its source prefix.
            if(!refreshSceneWorld49P(graph,model))return {S::graph,0x28b550u};
            const auto& coldWorld=graph.nodes[modelIndex].world100;
            QuerySpherePrepareOutput48O sphere{};
            if(!prepareType2(assets.queryE0(),coldWorld,sphere))return {S::preparation,0x11f680u};
            next->queries[0].type2World80=sphere.world;
            states[0].cachedCenter=sphere.center;states[0].cachedRadius=sphere.radius;
            auto& shape=next->queries[1].type0;shape.world80=coldWorld;shape.previousC0=coldWorld;
            if(!preparePodShape50IZ(assets.targetE4().source,coldWorld,shape))return {S::preparation,0x11f680u};
            states[1].cachedCenter=shape.center60;states[1].cachedRadius=shape.radius80;
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
    bool prepared()const noexcept{return bool(storage_);}
    bool constructed()const noexcept {
        return storage_&&storage_->published[0]&&storage_->published[1]&&storage_->published[2]&&
            storage_->contactBound&&storage_->mutualBound&&storage_->derivedBound;
    }
    // Source165E90 constructor publication. Component141300 has already run
    // on this actual +8 subobject, so flags31 borrows its authoritative29.
    // Source-specific later fields (2955F0 and2CB5A0) are separate methods.
    OriginalWreckQueryResult50LA publishColdQuery(unsigned i,OriginalWreckComponent50LA& component) noexcept {
        using S=OriginalWreckQueryStatus50LA;
        if(!storage_||i>2||storage_->published[i]||
           component.kind!=OriginalWreckComponentKind50LA::collision11B190||
           !component.objectKey||!component.sessionGeneration||
           !storage_->physical->bodyConstructorFieldsApplied||
           (i&& !storage_->published[i-1u])||(i==1&&!storage_->contactBound))
            return {S::binding,0x165e90u};
        auto& w=storage_->world;const auto index=w.refs[i]-1u;
        const auto& registered=w.lifecycle->storage.registrations[index];
        if(registered.owner||registered.slot||registered.encodedObject)return {S::binding,0x11b190u};
        auto definition=storage_->coldDefinitions[i];definition.filter.flags31=component.flags29;
        w.objects[index]=definition;w.states[index]=storage_->coldStates[i];
        w.copies[index].copied40=*w.sentinel3E02C0;w.copies[index].copied50=*w.sentinel3E02C0;
        w.geometry[index]={};
        if(i==1)w.geometry[index]={&storage_->assets->targetE4().mesh,query(1).type0.composed00,nullptr,true};
        *w.globalValid3E0730=0; //Actual11B570 invalidates the same application cache byte.
        storage_->components[i]=&component;storage_->published[i]=true;
        return {S::complete,0u};
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
        for(unsigned i=0;i<2;++i)object(i).filter.identities[0]={storage_->exclusions[i].data(),2,2,1};
        storage_->mutualBound=true;return true;
    }
    // Actual2CB5A0 after165C30 completes; the earlier base callback165920 is
    // never misrepresented as the derived response before this source store.
    bool bindDerivedResponse() noexcept {
        if(!coherent(1)||!storage_->published[2]||!storage_->mutualBound||storage_->derivedBound)return false;
        query(1).responseE8=ResponseCallback::wreck2CB420;storage_->derivedBound=true;return true;
    }
    Query& query(unsigned i) noexcept{return storage_->queries[i];}
    const Query& query(unsigned i)const noexcept{return storage_->queries[i];}
    CollectionObject50BX& object(unsigned i) noexcept{return storage_->world.objects[storage_->world.refs[i]-1u];}
    const CollectionObject50BX& object(unsigned i)const noexcept{return storage_->world.objects[storage_->world.refs[i]-1u];}
    CollectionObjectState50BX& state(unsigned i) noexcept{return storage_->world.states[storage_->world.refs[i]-1u];}
    const CollectionObjectState50BX& state(unsigned i)const noexcept{return storage_->world.states[storage_->world.refs[i]-1u];}
    CollectionRef50BX ref(unsigned i)const noexcept{return storage_->world.refs[i];}
    OriginalCameraWorldObjectCopies& copies(unsigned i) noexcept{return storage_->world.copies[ref(i)-1u];}
    const OriginalCameraWorldObjectCopies& copies(unsigned i)const noexcept{return storage_->world.copies[ref(i)-1u];}
    const OriginalWreckQueryWorld50LA& worldBinding()const noexcept{return storage_->world;}

    //22C214..22C258 adds the actual named WATER bit4342A8 to E0 and E8
    //category exclusions after2CB5A0 completes.124170 preserves first-match
    //order and does not append a duplicate. Storage remains distinct perD8.
    bool applyCollectionWaterExclusion(const std::uint32_t& actualWater4342A8) noexcept {
        if(!coherent(0)||!coherent(2)||storage_->collectionFilterApplied||!actualWater4342A8)return false;
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
            if(!preparePodShape50IZ(storage_->assets->targetE4().source,world,q.type0))return {S::preparation,0x11f680u};
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
        if(stageQueryPairs48O(in,q.endpoints,2,q.endpointMetadata,2,tag)!=QueryPairStagingStatus48O::Staged)
            return {S::preparation,0x11ca80u};
        return {S::complete,0u,true,false};
    }
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
        if(!storage_||i>2||!storage_->published[i])return false;const auto& w=storage_->world;const auto& q=query(i);
        if(object(i).identity!=w.identities[i]||object(i).shapeTag!=(i==0?2u:i==1?0u:3u)||
           object(i).filter.categoryA4!=*w.physicsBodyCategory3E0718||
           object(i).typeTag!=*w.collisionTypeGp8010||!object(i).hasShape)return false;
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
        OriginalWreckAssets50LA* assets=nullptr;
        OriginalOnePieceWreckOwner50LA* piece=nullptr;
        OriginalWreckPhysical50LA* physical=nullptr;
        OriginalWreckQueryWorld50LA world{};
        std::size_t capacity=0;
        std::array<std::array<std::uint32_t,2>,2> exclusions{};
        std::array<std::uint32_t,2> categoryD8{};
        std::uint32_t waterCategoryE0=0;
        bool collectionFilterApplied=false;
        bool published[3]{},contactBound=false,mutualBound=false,derivedBound=false;
        std::array<OriginalWreckComponent50LA*,3> components{};
        std::array<CollectionObject50BX,3> coldDefinitions{};
        std::array<CollectionObjectState50BX,3> coldStates{};
        std::array<Query,3> queries;
    };
    std::unique_ptr<Storage> storage_;
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
        else if(i==1){b.bounds.shapeWorld=q.type0.composed00;b.type0Resource=&storage_->assets->targetE4().source;}
        else {b.bounds.pairCount=1;b.bounds.endpoints=q.endpoints;b.bounds.endpointCount=2;
            b.bounds.endpointScenes=q.endpointMetadata;b.bounds.endpointSceneCount=2;}
        return b;
    }
    void publishBounds(unsigned i) noexcept {
        auto& cache=state(i);const auto& membership=query(i).membership;
        cache.cachedCenter=membership.bounds.cachedCenter;cache.cachedRadius=membership.bounds.cachedRadius;
        // No F8, iterator, selected hit or collision40/50 state is overwritten.
    }
};
} // namespace rr::game

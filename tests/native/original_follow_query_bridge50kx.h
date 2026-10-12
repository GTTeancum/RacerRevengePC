#pragma once
#include "track_runtime_storage50ig_fixture.h"
#include "../../src/game/original_camera_query_cold.h"
#include <algorithm>
#include <array>
#include <bit>
#include <cstring>
#include <limits>
#include <stdexcept>

// Native ownership join for the real Follow sphere24 and ray190. Construct
// the fixed backing cold BEFORE Bank::seal. Every other operation, including
// preparation and close, belongs inside the SAME Bank::visit(0) lease as the
// actual main690 query, candidate array and Storage execution arena. Do not
// bank this owner: the source camera exists once, observing racer slot zero.
namespace original_follow_query_bridge {
using namespace rr::game;
namespace storage=track_runtime_storage50IG;

struct Diagnostic {
    const char* message="Follow query bridge has not been prepared";
    std::uint32_t sourcePC=0;
    OriginalCameraQueryColdStatus cold=OriginalCameraQueryColdStatus::Binding;
    OriginalCameraPairResult pair{};
    LiveZoneResult50EN removal{};
};

class QueryBridge {
    storage::Storage& storage_;
    Type2StaticQuery49X& main_;
    CollisionFilter48J& mainFilter_;
    Type2StaticQuery49X& secondary_;
    CollisionFilter48J& secondaryFilter_;
    TerrainQuery48J& terrain_;
    storage::Membership sphereMembership_,rayMembership_;
    storage::FixedBuffer<OriginalCameraWorldGeometry> geometry_;
    storage::FixedBuffer<OriginalCameraWorldObjectCopies> copies_,stagedCopies_;
    storage::FixedBuffer<CollectionObjectState50BX> stagedObjects_;
    storage::FixedBuffer<CollectionTreeState50BX> stagedMethods_;
    storage::FixedBuffer<std::uint32_t> meshStack_;
    OriginalCameraQueryPairState state_{};
    OriginalCameraQueryPairOwner owner_{};
    Diagnostic diagnostic_{};
    // Native opaque collision identities assigned to THESE real object
    // records, never guest pointers, scene keys or generated hit receipts.
    static constexpr std::uint32_t sphereIdentity=0xf0000004u,rayIdentity=0xf0000005u;
    //270A1C reads SceneInfo.UseEnvironmentNode with default0;272F28 reaches
    // the lazy443108 environment owner only when it is nonzero. This bridge
    // admits only the actual F/default authored route, checked below. A T
    // needs the genuine shared443A98 spatial method, not an empty fake list.
    const CollectionRef50BX globalFallbackZone443108_=0;
    // Initialized application source constants3C8220 and3C966C. There is no
    // mutable private substitute for their initialized (+0,+0,+0,1) values.
    inline static constexpr SpringVector4_48N sentinel3E02C0{{0.f,0.f,0.f,1.f}};
    inline static constexpr SpringVector4_48N zero433DB0{{0.f,0.f,0.f,1.f}};
    bool attempted_=false,ready_=false,synced_=false,closed_=false;

    static std::size_t meshCapacity(const storage::Storage& s) {
        std::size_t result=1;
        const auto include=[&](const rr::assets::CollisionMesh48I* mesh){
            if(!mesh)return;
            if(mesh->nodes.size()>=std::numeric_limits<std::size_t>::max()-mesh->faces.size())
                throw std::runtime_error("Follow query mesh-stack extent overflow");
            result=std::max(result,mesh->nodes.size()+mesh->faces.size()+1u);
        };
        for(std::size_t i=0;i<s.count.terrain;++i){
            const auto* candidate=s.candidates[i];
            if(!candidate)throw std::runtime_error("Follow query candidate owner missing");
            include(candidate->candidate.collision.geometry.mesh);
            const auto* compound=candidate->compound;
            if(compound){
                if(!compound->parts||compound->count>maxCompoundMembers50IJ)
                    throw std::runtime_error("Follow query compound owner incomplete");
                for(std::size_t j=0;j<compound->count;++j)include(compound->parts[j].mesh);
            }
        }
        return result;
    }
    static void admitNoEnvironmentFallback(const storage::Ground& ground) {
        const auto& world=ground.world;
        if(world.zones.empty()||world.scene_info_block>=world.parameters.blocks.size())
            throw std::runtime_error("Follow query original SceneInfo is unavailable");
        const auto& info=world.parameters.blocks[world.scene_info_block];
        if(info.name!="sceneinfo")throw std::runtime_error("Follow query SceneInfo identity differs");
        bool seen=false;
        for(const auto& field:info.fields)if(field.name=="useenvironmentnode"){
            if(seen||field.value!="F")
                throw std::runtime_error("Follow query UseEnvironmentNode requires the real unsupported443A98 owner");
            seen=true;
        }
    }
    bool fail(const char* text,std::uint32_t pc) noexcept {
        diagnostic_.message=text;diagnostic_.sourcePC=pc;return false;
    }
    bool bindings() const noexcept {
        const auto& n=storage_.count;
        return n.cameraSphere==n.terrain+4u&&n.cameraRay==n.terrain+5u&&
            n.objects==n.terrain+5u&&n.objects<=storage::runtimeSearchSlotLimit&&
            storage_.objects.size()==n.objects&&storage_.source.size()==n.objects&&
            main_.filter==&mainFilter_&&secondary_.filter==&secondaryFilter_&&
            main_.collisionTypeTag!=0xffu&&main_.collisionTypeTag==secondary_.collisionTypeTag&&
            main_.collisionTypeTag==terrain_.collisionTypeTag&&
            main_.materialMask&&!(main_.materialMask&(main_.materialMask-1u))&&
            secondary_.materialMask==main_.materialMask&&
            storage_.lifecycle.objectCount==n.objects&&storage_.lifecycle.zoneCount==n.zones&&
            storage_.world.zoneCount==n.zones;
    }
    static bool member(const LiveZoneMembership50EN& membership,CollectionRef50BX ref,
        const storage::Storage& s) noexcept {
        if(!membership.attached||membership.object!=ref||membership.method!=ZoneMethod50CL::Second||
           membership.lastShapeTag!=2||!membership.storage.links||!membership.storage.entries||
           !membership.storage.capacity||!membership.storage.entries[0].used)return false;
        const auto zone=membership.storage.links[0].zone;
        if(!zone||zone>s.count.zones)return false;
        const auto& record=membership.storage.entries[0].registration;
        const auto& actual=s.registrations[ref-1u];
        return record.owner&&record.owner==actual.owner&&record.slot==actual.slot&&
            record.encodedObject==actual.encodedObject&&record.encodedObject==(ref<<2u);
    }
    bool springBinding(const Type2StaticQuery49X& query,const LiveZoneMembership50EN& membership,
        CollectionRef50BX ref) const noexcept {
        return member(membership,ref,storage_)&&query.resource&&
            query.resource==membership.lastType2Resource&&
            !std::memcmp(query.worldMatrix.v,membership.lastShapeWorld.v,sizeof query.worldMatrix.v)&&
            !std::memcmp(query.sphere.center.v,membership.bounds.cachedCenter.v,sizeof query.sphere.center.v)&&
            !std::memcmp(&query.sphere.radius,&membership.bounds.cachedRadius,sizeof query.sphere.radius);
    }
    void project(std::size_t index,const CollectionProjection50CF& projection,
        const std::array<float,4>& copied40,const std::array<float,4>& copied50) noexcept {
        storage_.objects[index]=projection.object;
        // These projections rebind actual filter/cache lanes, not collector
        // history. Never reset the authoritative object epoch/member state.
        auto& actual=storage_.source[index];
        actual.flagsF8=projection.state.flagsF8;
        actual.cachedCenter=projection.state.cachedCenter;
        actual.cachedRadius=projection.state.cachedRadius;
        std::memcpy(copies_[index].copied40.v,copied40.data(),sizeof copies_[index].copied40.v);
        std::memcpy(copies_[index].copied50.v,copied50.data(),sizeof copies_[index].copied50.v);
    }
    bool publishQuery(CollectionRef50BX ref,const CollisionFilter48J& filter,
        const LiveZoneMembership50EN& membership,std::uint8_t tag,
        const std::array<float,4>& copied40,const std::array<float,4>& copied50) noexcept {
        CollisionSphere48J sphere{};
        std::memcpy(sphere.center.v,membership.bounds.cachedCenter.v,sizeof sphere.center.v);
        sphere.radius=membership.bounds.cachedRadius;
        const auto projection=projectCollectionObject50CF(filter,sphere,tag,2,
            std::bit_cast<std::uint32_t>(membership.bounds.cachedCenter.v[3]));
        if(!projection.valid)return false;
        project(ref-1u,projection,copied40,copied50);
        geometry_[ref-1u]={};geometry_[ref-1u].world=membership.lastShapeWorld;
        return true;
    }
    bool refreshRecords() noexcept {
        if(!bindings()||!springBinding(main_,storage_.mainMembership.state,storage_.count.mainQuery)||
           !springBinding(secondary_,storage_.secondaryMembership.state,storage_.count.secondaryQuery)||
           !member(storage_.terrainMembership.state,storage_.count.terrainQuery,storage_))
            return fail("Follow query actual690/secondary/terrain binding differs",0x1751d4u);
        const auto unique=[&](std::uint32_t id,std::size_t before) noexcept {
            if(!id||id==sphereIdentity||id==rayIdentity)return false;
            for(std::size_t j=0;j<before;++j)if(storage_.candidates[j]->candidate.collision.filter.identity==id)return false;
            return true;
        };
        for(std::size_t i=0;i<storage_.count.terrain;++i){
            auto* candidate=storage_.candidates[i];
            if(!candidate)return fail("Follow query concrete candidate missing",0x121b68u);
            const auto& collision=candidate->candidate.collision;
            if(!unique(collision.filter.identity,i)||collision.filter.identity!=collision.geometry.id||
               collision.typeTag!=main_.collisionTypeTag||
               (candidate->shapeType==0&&!collision.geometry.mesh)||
               (candidate->shapeType==1&&(!candidate->compound||candidate->compound!=collision.compound)))
                return fail("Follow query concrete candidate identity or geometry differs",0x126f40u);
        }
        const std::uint32_t ids[]{mainFilter_.identity,secondaryFilter_.identity,terrain_.filter.identity};
        for(unsigned i=0;i<3;++i){
            if(!unique(ids[i],storage_.count.terrain))return fail("Follow query live identity collision",0x11b190u);
            for(unsigned j=0;j<i;++j)if(ids[i]==ids[j])return fail("Follow query live identity collision",0x11b190u);
        }
        for(std::size_t i=0;i<storage_.count.terrain;++i){
            const auto& candidate=*storage_.candidates[i];const auto& collision=candidate.candidate.collision;
            const auto projection=projectCollectionObject50CF(collision.filter,collision.sphere,
                collision.typeTag,candidate.shapeType,std::bit_cast<std::uint32_t>(storage_.source[i].cachedCenter.v[3]));
            if(!projection.valid)return fail("Follow query candidate projection rejected",0x120e70u);
            project(i,projection,collision.copied40,collision.copied50);
            geometry_[i]={collision.geometry.mesh,collision.geometry.worldMatrix,candidate.compound,
                collision.hasResponseCallbackE8};
        }
        if(!publishQuery(storage_.count.mainQuery,mainFilter_,storage_.mainMembership.state,
                main_.collisionTypeTag,main_.copied40,main_.copied50)||
           !publishQuery(storage_.count.secondaryQuery,secondaryFilter_,storage_.secondaryMembership.state,
                secondary_.collisionTypeTag,secondary_.copied40,secondary_.copied50)||
           !publishQuery(storage_.count.terrainQuery,terrain_.filter,storage_.terrainMembership.state,
                terrain_.collisionTypeTag,terrain_.copied40,terrain_.copied50))
            return fail("Follow query actual query projection rejected",0x120e70u);
        return true;
    }
    void commitCopies(std::size_t index,std::array<float,4>& copied40,
        std::array<float,4>& copied50) const noexcept {
        std::memcpy(copied40.data(),copies_[index].copied40.v,sizeof copies_[index].copied40.v);
        std::memcpy(copied50.data(),copies_[index].copied50.v,sizeof copies_[index].copied50.v);
    }
public:
    QueryBridge(storage::Storage& track,const storage::Ground& ground,
        CollisionNormalModeOwner50JA& normalMode,Type2StaticQuery49X& main,CollisionFilter48J& mainFilter,
        Type2StaticQuery49X& secondary,CollisionFilter48J& secondaryFilter,TerrainQuery48J& terrainMetadata):
        storage_(track),main_(main),mainFilter_(mainFilter),secondary_(secondary),secondaryFilter_(secondaryFilter),
        terrain_(terrainMetadata),sphereMembership_(track.count.membership),rayMembership_(track.count.membership),
        geometry_(track.count.objects),copies_(track.count.objects),stagedCopies_(track.count.objects),
        stagedObjects_(track.count.objects),stagedMethods_(track.count.methods),meshStack_(meshCapacity(track)) {
        admitNoEnvironmentFallback(ground);
        if(!bindings())throw std::runtime_error("Follow query bridge requires the actual reserved cold Storage and query owners");
        sphereMembership_.state.storage=sphereMembership_.storage();
        rayMembership_.state.storage=rayMembership_.storage();
        owner_.lifecycle=&storage_.lifecycle;owner_.liveWorld=&storage_.world;
        owner_.liveWorkspace=&storage_.membershipWork;
        owner_.sphereE0=&sphereMembership_.state;owner_.rayE0=&rayMembership_.state;
        owner_.sphereObject=storage_.count.cameraSphere;owner_.rayObject=storage_.count.cameraRay;
        owner_.state=&state_;owner_.normalMode3DF5B0=&normalMode;
        auto& world=owner_.world;
        world.objects=storage_.objects.data();world.geometry=geometry_.data();world.objectCount=storage_.count.objects;
        world.methods=storage_.trees.data();world.methodCount=storage_.count.methods;
        world.zones=storage_.zones.data();world.zoneCount=storage_.count.zones;
        world.globalFallbackZone443108=&globalFallbackZone443108_;
        //11B270..11B290 resolves the same CollisionObject registry name used
        // by the existing main690 owner.3C83C4..3C8448 resolves STATICOBJECT,
        // the same named native category already carried by materialMask.
        world.directTypeTagGp8010=&main_.collisionTypeTag;world.staticObjectMask3E0720=&main_.materialMask;
        owner_.mutableWorld={storage_.source.data(),storage_.count.objects,
            storage_.treeSource.data(),storage_.count.methods,copies_.data(),copies_.size()};
        owner_.queryWorkspace={{stagedObjects_.data(),stagedObjects_.size(),stagedMethods_.data(),
            stagedMethods_.size(),stagedCopies_.data(),stagedCopies_.size()},meshStack_.data(),meshStack_.size()};
    }
    QueryBridge(const QueryBridge&)=delete;QueryBridge& operator=(const QueryBridge&)=delete;
    QueryBridge(QueryBridge&&)=delete;QueryBridge& operator=(QueryBridge&&)=delete;
    const Diagnostic& diagnostic() const noexcept{return diagnostic_;}
    OriginalCameraQueryPairOwner& pairOwner() noexcept{return owner_;}
    bool prepared() const noexcept{return ready_;}

    // Join the accepted ordinary vehicle collector's F8 aliases. Call at its
    // existing successful publication point, before another query. The active
    // BANKED domain is checked first: opponent visits do not even read this
    // unbanked camera's state or memberships.
    bool publishCollection(const CollectionObjectState50BX* accepted,std::size_t activeCount) noexcept {
        const auto actualCount=storage_.collectionInput.objectCount;
        if(activeCount!=actualCount)return fail("Follow query collector extent differs",0x1223e4u);
        if(actualCount==storage_.count.terrain+storage::runtimeDynamicReferenceReserve)return true;
        if(actualCount!=storage_.count.objects||!accepted||!ready_||closed_||synced_||
           !sphereMembership_.state.attached||!rayMembership_.state.attached||
           storage_.objects[owner_.sphereObject-1u].identity!=sphereIdentity||
           storage_.objects[owner_.rayObject-1u].identity!=rayIdentity)
            return fail("Follow query collector publication is outside bank0",0x1223e4u);
        const auto sphere=owner_.sphereObject-1u,ray=owner_.rayObject-1u;
        storage_.source[sphere].flagsF8=accepted[sphere].flagsF8;
        storage_.source[ray].flagsF8=accepted[ray].flagsF8;
        state_.sphere24.flagsF8=storage_.source[sphere].flagsF8;
        state_.ray190.flagsF8=storage_.source[ray].flagsF8;
        return true;
    }

    // Call only once, inside bank0, after the real Follow authoring produced
    // radius178 and after main690 completed its actual initial registration.
    bool prepare(float radius178) noexcept {
        if(attempted_||closed_)return fail("Follow query cold preparation cannot be replayed",0x179a80u);
        attempted_=true;
        if(storage_.collectionInput.objectCount!=storage_.count.terrain+storage::runtimeDynamicReferenceReserve)
            return fail("Follow query initial bank0 publication rejected",0x179a80u);
        if(!refreshRecords())return false;
        const auto& mainMembership=storage_.mainMembership.state;
        //1751F4 passes racer690+60,1751FC passes its actual primary E0->4.
        // Preserve all four cache lanes; never use racer body origin or a
        // position-to-zone guess. The springBinding check authenticates XYZ.
        const auto position=mainMembership.bounds.cachedCenter;
        const auto initialZone=mainMembership.storage.links[0].zone;
        std::array<CollectionObject50BX,2> objects{};
        objects[0].identity=sphereIdentity;objects[1].identity=rayIdentity;
        const std::array<CollectionObjectState50BX,2> prior{};
        const OriginalCameraQueryColdInput input{radius178,position,sentinel3E02C0,zero433DB0,main_.collisionTypeTag};
        const auto projection=projectOriginalCameraQueryCold(input,state_,objects,prior);
        diagnostic_.cold=projection.status;
        if(projection.status!=OriginalCameraQueryColdStatus::Complete)
            return fail("Follow query source cold constructor projection rejected",projection.sourcePC);
        state_=projection.pair;
        const CollectionRef50BX refs[]{owner_.sphereObject,owner_.rayObject};
        for(unsigned i=0;i<2;++i){
            const auto index=refs[i]-1u;storage_.objects[index]=projection.objects[i];
            storage_.source[index]=projection.collection[i];copies_[index]=projection.copies[i];geometry_[index]={};
            geometry_[index].world.v[0]=geometry_[index].world.v[5]=
                geometry_[index].world.v[10]=geometry_[index].world.v[15]=1.f;
        }
        // This field is banked. Opponent domains remain their original +3.
        storage_.collectionInput.objectCount=storage_.count.objects;
        const auto status=attachOriginalCameraQueryPair(owner_,initialZone,position,diagnostic_.pair);
        if(status!=OriginalCameraPairStatus::Complete)
            return fail("Follow query real sphere/ray120070 attachment rejected",diagnostic_.pair.sourcePC);
        ready_=true;diagnostic_.message="Follow query pair attached";diagnostic_.sourcePC=0;return true;
    }
    // refreshAnimatedCandidates must have run in this SAME bank0 visit first.
    // Keep this lease open through Follow advancement and commit(), including
    // failure, so completed source prefixes never disappear in a copy shadow.
    bool sync() noexcept {
        if(!ready_||closed_||synced_||storage_.collectionInput.objectCount!=storage_.count.objects||
           storage_.objects[owner_.sphereObject-1u].identity!=sphereIdentity||
           storage_.objects[owner_.rayObject-1u].identity!=rayIdentity)
            return fail("Follow query sync is outside its ready bank0 domain",0x17a870u);
        if(!refreshRecords())return false;
        state_.sphere24.flagsF8=storage_.source[owner_.sphereObject-1u].flagsF8;
        state_.ray190.flagsF8=storage_.source[owner_.rayObject-1u].flagsF8;
        copies_[owner_.sphereObject-1u]={state_.sphere24.copied40,state_.sphere24.copied50};
        copies_[owner_.rayObject-1u]={state_.ray190.copied40,state_.ray190.copied50};
        synced_=true;diagnostic_.message="Follow query actual world synchronized";diagnostic_.sourcePC=0;return true;
    }
    bool commit() noexcept {
        if(!synced_||!ready_||closed_||storage_.collectionInput.objectCount!=storage_.count.objects)
            return fail("Follow query commit has no matching bank0 sync",0x121920u);
        for(std::size_t i=0;i<storage_.count.terrain;++i)
            if(!storage_.candidates[i]||storage_.objects[i].identity!=storage_.candidates[i]->candidate.collision.filter.identity)
                return fail("Follow query candidate owner changed during lease",0x121b68u);
        for(std::size_t i=0;i<storage_.count.terrain;++i){
            auto& actual=storage_.candidates[i]->candidate.collision;
            auto& movement=storage_.movement[i].collision;
            actual.filter.flagsF8=storage_.source[i].flagsF8;
            commitCopies(i,actual.copied40,actual.copied50);
            movement.filter.flagsF8=actual.filter.flagsF8;
            movement.copied40=actual.copied40;movement.copied50=actual.copied50;
        }
        mainFilter_.flagsF8=storage_.source[storage_.count.mainQuery-1u].flagsF8;
        secondaryFilter_.flagsF8=storage_.source[storage_.count.secondaryQuery-1u].flagsF8;
        terrain_.filter.flagsF8=storage_.source[storage_.count.terrainQuery-1u].flagsF8;
        commitCopies(storage_.count.mainQuery-1u,main_.copied40,main_.copied50);
        commitCopies(storage_.count.secondaryQuery-1u,secondary_.copied40,secondary_.copied50);
        commitCopies(storage_.count.terrainQuery-1u,terrain_.copied40,terrain_.copied50);
        synced_=false;diagnostic_.message="Follow query source-prefix writes committed to actual owners";
        diagnostic_.sourcePC=0;return true;
    }
    // Explicit teardown belongs inside bank0 while all external collision
    // owners still live. A failed removal retains its exact successful prefix;
    // retry continues with whichever real membership remains attached.
    bool close() noexcept {
        if(closed_)return true;
        if((sphereMembership_.state.attached||rayMembership_.state.attached)&&
           storage_.collectionInput.objectCount!=storage_.count.objects)
            return fail("Follow query removal is outside bank0",0x120290u);
        if(synced_&&!commit())return false;
        for(auto* membership:{&sphereMembership_.state,&rayMembership_.state})if(membership->attached){
            diagnostic_.removal=removeLiveZoneMembership50EN(storage_.lifecycle,storage_.world,
                *membership,storage_.membershipWork);
            if(diagnostic_.removal.status!=LiveZoneStatus50EN::Complete)
                return fail("Follow query real120290 removal rejected",0x120290u);
        }
        storage_.collectionInput.objectCount=storage_.count.terrain+storage::runtimeDynamicReferenceReserve;
        ready_=false;closed_=true;diagnostic_.message="Follow query memberships removed";
        diagnostic_.sourcePC=0;return true;
    }
};
} // namespace original_follow_query_bridge

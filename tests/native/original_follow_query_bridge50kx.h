#pragma once
#include "track_runtime_storage50ig_fixture.h"
#include "undertrack_owners50ew_fixture.h"
#include "native_query_identities50kx.h"
#include "../../src/game/original_camera_query_cold.h"
#include "../../src/game/original_death_camera_query.h"
#include "../../src/game/original_wreck_queries50la.h"
#include <algorithm>
#include <array>
#include <bit>
#include <cstring>
#include <limits>
#include <stdexcept>

// Planned same-source RA runtime owner; only a declaration, no runtime binding.
namespace ra_source_camera50LA { class OwnershipBridge; }

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
    friend class ra_source_camera50LA::OwnershipBridge;
    storage::Storage& storage_;
    Type2StaticQuery49X& main_;
    CollisionFilter48J& mainFilter_;
    Type2StaticQuery49X& secondary_;
    CollisionFilter48J& secondaryFilter_;
    TerrainQuery48J& terrain_;
    // Optional borrows of the sole banked694 and existing7A4 owners. The
    // camera remains unbanked; these owners are accessed only inside bank0.
    undertrack_owners50ew_fixture::Owners* recovery_=nullptr;
    const VehicleFrameState49W* frame_=nullptr;
    Type3QueryInput48P* movement_=nullptr;
    std::size_t recoveryMeshStackCapacity_=0;
    std::size_t wreckMeshStackCapacity_=0;
    struct WreckBinding {
        OriginalWreckQueries50LA* owner=nullptr;
        const OriginalWreckAssets50LA* assets=nullptr;
        std::array<std::uint32_t,3> identities{};
    };
    storage::FixedBuffer<WreckBinding> wreck_;
    storage::Membership sphereMembership_,rayMembership_,deathMembership_;
    storage::FixedBuffer<OriginalCameraWorldGeometry> geometry_;
    storage::FixedBuffer<OriginalCameraWorldObjectCopies> copies_,stagedCopies_;
    storage::FixedBuffer<CollectionObjectState50BX> stagedObjects_;
    storage::FixedBuffer<CollectionTreeState50BX> stagedMethods_;
    storage::FixedBuffer<std::uint32_t> meshStack_;
    OriginalCameraQueryPairState state_{};
    OriginalCameraQueryPairOwner owner_{};
    OriginalDeathCameraQueryOwner deathOwner_{};
    OriginalDeathCameraQueryColdOwner deathCold_{};
    Diagnostic diagnostic_{};
    // Native opaque collision identities assigned to THESE real object
    // records, never guest pointers, scene keys or generated hit receipts.
    static constexpr std::uint32_t sphereIdentity=native_query_identities50KX::followSphere24,
        rayIdentity=native_query_identities50KX::followRay190,deathIdentity=native_query_identities50KX::death120;
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

    static bool meshExtent(const rr::assets::CollisionMesh48I& mesh,std::size_t& extent) noexcept {
        const auto maximum=std::numeric_limits<std::size_t>::max();
        if(mesh.faces.size()==maximum||mesh.nodes.size()>maximum-mesh.faces.size()-1u)return false;
        extent=mesh.nodes.size()+mesh.faces.size()+1u;return true;
    }
    static std::size_t meshCapacity(const storage::Storage& s,std::size_t recoveryCapacity) {
        std::size_t result=std::max(std::size_t{1},recoveryCapacity);
        const auto include=[&](const rr::assets::CollisionMesh48I* mesh){
            if(!mesh)return;
            std::size_t extent=0;if(!meshExtent(*mesh,extent))
                throw std::runtime_error("Follow query mesh-stack extent overflow");
            result=std::max(result,extent);
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
    static void admitNoEnvironmentFallback(const rr::assets::World48F& world) {
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
        const bool recoveryBindings=recovery_?
            frame_&&movement_&&recoveryMeshStackCapacity_&&n.recoveryReferences==2u&&
                n.podQuery==n.terrain+4u&&n.movementQuery==n.terrain+5u:
            !frame_&&!movement_&&!recoveryMeshStackCapacity_&&!n.recoveryReferences&&!n.podQuery&&!n.movementQuery;
        const bool wreckBindings=n.wreckReferences?
            n.wreckFirstQuery==n.cameraDeath+1u&&n.wreckReferences%3u==0u&&
                wreck_.size()==n.wreckReferences/3u&&wreckMeshStackCapacity_&&
                n.objects==n.cameraDeath+n.wreckReferences:
            !n.wreckFirstQuery&&!wreck_.size()&&!wreckMeshStackCapacity_&&n.objects==n.cameraDeath;
        return recoveryBindings&&wreckBindings&&n.mainQuery==n.terrain+1u&&n.secondaryQuery==n.terrain+2u&&
            n.terrainQuery==n.terrain+3u&&n.cameraSphere==n.terrain+4u+n.recoveryReferences&&
            n.cameraRay==n.terrain+5u+n.recoveryReferences&&
            n.cameraDeath==n.terrain+6u+n.recoveryReferences&&
            n.objects>=n.cameraDeath&&n.objects<=storage::runtimeSearchSlotLimit&&
            storage_.objects.size()==n.objects&&storage_.source.size()==n.objects&&
            main_.filter==&mainFilter_&&secondary_.filter==&secondaryFilter_&&
            main_.collisionTypeTag!=0xffu&&main_.collisionTypeTag==secondary_.collisionTypeTag&&
            main_.collisionTypeTag==terrain_.collisionTypeTag&&
            main_.materialMask&&!(main_.materialMask&(main_.materialMask-1u))&&
            secondary_.materialMask==main_.materialMask&&
            storage_.lifecycle.objectCount==n.objects&&storage_.lifecycle.zoneCount==n.zones&&
            storage_.world.zoneCount==n.zones;
    }
    std::size_t nonCameraCount() const noexcept {
        return storage_.count.terrain+storage::runtimeDynamicReferenceReserve+storage_.count.recoveryReferences;
    }
    static bool member(const LiveZoneMembership50EN& membership,CollectionRef50BX ref,
        const storage::Storage& s,std::uint32_t shapeTag=2) noexcept {
        if(!membership.attached||membership.object!=ref||membership.method!=ZoneMethod50CL::Second||
           membership.lastShapeTag!=shapeTag||!membership.storage.links||!membership.storage.entries||
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
    bool recoveryBindings() const noexcept {
        if(!recovery_)return true;
        const auto& actual=*recovery_;const auto& shape=frame_->shape;
        if(!actual.storagePrepared||!actual.sourcePrepared||!actual.selectedSource||
           !actual.selectedSource->prepared()||main_.resource!=&actual.selectedSource->main()||
           secondary_.resource!=&actual.selectedSource->secondary()||
           actual.globals.collisionTypeTag!=main_.collisionTypeTag||
           movement_->collisionTypeTag!=main_.collisionTypeTag||movement_->pairCount!=1u||
           actual.podFilter.identity!=actual.globals.identity[0]||
           movement_->filter.identity!=actual.globals.identity[1]||
           shape.type88!=0||shape.registrationE0!=storage_.count.podQuery||actual.movementTag!=3||
           !member(actual.memberships[0],storage_.count.podQuery,storage_,0)||
           !member(actual.memberships[1],storage_.count.movementQuery,storage_,3))return false;
        for(unsigned i=0;i<2;++i){const auto& m=actual.memberships[i];
            if(m.storage.links!=actual.links[i].data()||m.storage.entries!=actual.entries[i].data()||
               m.storage.capacity!=actual.links[i].size()||m.storage.capacity!=actual.entries[i].size())return false;}
        const auto& pod=actual.memberships[0];const auto& movement=actual.memberships[1];
        std::size_t extent=0;
        return meshExtent(actual.selectedSource->mainTarget().mesh,extent)&&
            extent<=recoveryMeshStackCapacity_&&extent<=meshStack_.size()&&
            !std::memcmp(shape.composed00.v,pod.lastShapeWorld.v,sizeof shape.composed00.v)&&
            !std::memcmp(shape.center60.v,pod.bounds.cachedCenter.v,sizeof shape.center60.v)&&
            !std::memcmp(&shape.radius80,&pod.bounds.cachedRadius,sizeof shape.radius80)&&
            !std::memcmp(actual.movementSphere.center.v,movement.bounds.cachedCenter.v,
                sizeof actual.movementSphere.center.v)&&
            !std::memcmp(&actual.movementSphere.radius,&movement.bounds.cachedRadius,
                sizeof actual.movementSphere.radius);
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
    bool publishRecovery() noexcept {
        if(!recovery_)return true;
        const auto& actual=*recovery_;const auto& shape=frame_->shape;
        CollisionSphere48J podSphere{};
        std::memcpy(podSphere.center.v,shape.center60.v,sizeof podSphere.center.v);
        podSphere.radius=shape.radius80;
        const auto pod=projectCollectionObject50CF(actual.podFilter,podSphere,
            actual.globals.collisionTypeTag,shape.type88,std::bit_cast<std::uint32_t>(shape.center60.v[3]));
        const auto movement=projectCollectionObject50CF(movement_->filter,actual.movementSphere,
            movement_->collisionTypeTag,actual.movementTag,
            std::bit_cast<std::uint32_t>(actual.memberships[1].bounds.cachedCenter.v[3]));
        if(!pod.valid||!movement.valid)return false;
        const auto podIndex=storage_.count.podQuery-1u,movementIndex=storage_.count.movementQuery-1u;
        project(podIndex,pod,actual.podCopied40,actual.podCopied50);
        project(movementIndex,movement,movement_->copied40,movement_->copied50);
        //11B350 leaves this actual694 response E8 null. Its original type0
        // mesh is camera-visible; the selected source and canonical00 matrix
        // are borrowed directly, including after a live reset/reattachment.
        geometry_[podIndex]={&actual.selectedSource->mainTarget().mesh,shape.composed00,nullptr,false};
        //7A4 remains its actual query-only type3 object. Its filter is not
        // altered to hide it:121B68 may still set F8 bit4 before eligibility.
        geometry_[movementIndex]={};geometry_[movementIndex].world=actual.memberships[1].lastShapeWorld;
        return true;
    }
    bool refreshRecords() noexcept {
        if(!bindings()||!springBinding(main_,storage_.mainMembership.state,storage_.count.mainQuery)||
           !springBinding(secondary_,storage_.secondaryMembership.state,storage_.count.secondaryQuery)||
           !member(storage_.terrainMembership.state,storage_.count.terrainQuery,storage_))
            return fail("Follow query actual690/secondary/terrain binding differs",0x1751d4u);
        if(!recoveryBindings())
            return fail("Follow query actual694/7A4 owner, canonical bounds or cold mesh extent differs",0x126f40u);
        if(!wreckRecordsReady())
            return fail("Follow query genuine wreck query owners are incomplete or no longer bound",0x165e90u);
        const auto unique=[&](std::uint32_t id,std::size_t before) noexcept {
            if(!id||id==sphereIdentity||id==rayIdentity||id==deathIdentity)return false;
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
        const std::uint32_t ids[]{mainFilter_.identity,secondaryFilter_.identity,terrain_.filter.identity,
            recovery_?recovery_->podFilter.identity:0u,movement_?movement_->filter.identity:0u};
        const unsigned liveCount=recovery_?5u:3u;
        for(unsigned i=0;i<liveCount;++i){
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
                terrain_.collisionTypeTag,terrain_.copied40,terrain_.copied50)||!publishRecovery())
            return fail("Follow query actual query projection rejected",0x120e70u);
        return true;
    }
    // The tail is already canonical. Never project it through copied vehicle
    // or camera state, and never assign spare identities merely to satisfy the
    // world's nonzero-identity admission. Every reserved row must be an actual
    // constructed piece query before the full camera extent is published.
    bool wreckRecordsReady() const noexcept {
        for(std::size_t i=0;i<wreck_.size();++i){
            const auto& b=wreck_[i];if(!b.owner||!b.assets||!b.assets->prepared()||!b.owner->constructed())return false;
            const auto& w=b.owner->worldBinding();
            if(w.objects!=storage_.objects.data()||w.states!=storage_.source.data()||
               w.copies!=copies_.data()||w.geometry!=geometry_.data()||
               w.lifecycle!=&storage_.lifecycle||w.world!=&storage_.world||
               w.workspace!=&storage_.membershipWork||w.objectCount!=storage_.count.objects||
               w.globalValid3E0730!=&storage_.globalValid3E0730)return false;
            for(unsigned q=0;q<3;++q){
                const auto ref=static_cast<CollectionRef50BX>(storage_.count.wreckFirstQuery+i*3u+q);
                if(!b.owner->coherent(q)||b.owner->ref(q)!=ref||w.identities[q]!=b.identities[q]||
                   storage_.objects[ref-1u].identity!=b.identities[q])return false;
                for(std::size_t other=0;other<storage_.count.objects;++other)
                    if(other!=ref-1u&&storage_.objects[other].identity==b.identities[q])return false;
            }
            const auto index=b.owner->ref(1)-1u;const auto& shape=b.owner->query(1).type0;
            std::size_t extent=0;
            if(!meshExtent(b.assets->targetE4().mesh,extent)||extent>wreckMeshStackCapacity_||extent>meshStack_.size()||
               geometry_[index].mesh!=&b.assets->targetE4().mesh||geometry_[index].compound||
               !geometry_[index].hasResponseCallbackE8||
               std::memcmp(geometry_[index].world.v,shape.composed00.v,sizeof shape.composed00.v))return false;
        }
        return true;
    }
    void commitCopies(std::size_t index,std::array<float,4>& copied40,
        std::array<float,4>& copied50) const noexcept {
        std::memcpy(copied40.data(),copies_[index].copied40.v,sizeof copies_[index].copied40.v);
        std::memcpy(copied50.data(),copies_[index].copied50.v,sizeof copies_[index].copied50.v);
    }
    // Private same-source path for the future RA OwnershipBridge. World48F
    // is inspected only during construction; it does not supply geometry or
    // replace Storage. That bridge must retain the authenticated source and
    // its matching exclusive Storage world lease, and all actual query/filter/
    // normal-mode owners, through query retirement. No such RA owner exists
    // here, and construction neither prepares queries nor publishes a camera.
    QueryBridge(storage::Storage& track,const rr::assets::World48F& sourceWorld,
        CollisionNormalModeOwner50JA& normalMode,Type2StaticQuery49X& main,CollisionFilter48J& mainFilter,
        Type2StaticQuery49X& secondary,CollisionFilter48J& secondaryFilter,TerrainQuery48J& terrainMetadata,
        undertrack_owners50ew_fixture::Owners* recovery,const VehicleFrameState49W* frame,
        Type3QueryInput48P* movement,std::size_t recoveryMeshStackCapacity,
        std::size_t wreckMeshStackCapacity):
        storage_(track),main_(main),mainFilter_(mainFilter),secondary_(secondary),secondaryFilter_(secondaryFilter),
        terrain_(terrainMetadata),recovery_(recovery),frame_(frame),movement_(movement),
        recoveryMeshStackCapacity_(recoveryMeshStackCapacity),wreckMeshStackCapacity_(wreckMeshStackCapacity),
        wreck_(track.count.wreckReferences/3u),
        sphereMembership_(track.count.membership),rayMembership_(track.count.membership),
        deathMembership_(track.count.membership),
        geometry_(track.count.objects),copies_(track.count.objects),stagedCopies_(track.count.objects),
        stagedObjects_(track.count.objects),stagedMethods_(track.count.methods),
        meshStack_(meshCapacity(track,std::max(recoveryMeshStackCapacity,wreckMeshStackCapacity))) {
        admitNoEnvironmentFallback(sourceWorld);
        if(!bindings())throw std::runtime_error("Follow query bridge requires the actual reserved cold Storage and query owners");
        sphereMembership_.state.storage=sphereMembership_.storage();
        rayMembership_.state.storage=rayMembership_.storage();
        deathMembership_.state.storage=deathMembership_.storage();
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
        deathOwner_.lifecycle=owner_.lifecycle;deathOwner_.liveWorld=owner_.liveWorld;
        deathOwner_.liveWorkspace=owner_.liveWorkspace;deathOwner_.membershipE0=&deathMembership_.state;
        deathOwner_.object120=storage_.count.cameraDeath;deathOwner_.world=owner_.world;
        deathOwner_.mutableWorld=owner_.mutableWorld;deathOwner_.queryWorkspace=owner_.queryWorkspace;
        deathOwner_.normalMode3DF5B0=&normalMode;
        const auto deathIndex=storage_.count.cameraDeath-1u;
        // Reserve an actual collision-table identity only. It has no shape,
        // E0 or query120 and cannot occur in any zone until source22D8A8.
        storage_.objects[deathIndex].identity=deathIdentity;storage_.objects[deathIndex].hasShape=false;
        storage_.objects[deathIndex].shapeTag=0;
        deathCold_.identity120=deathIdentity;deathCold_.object120=&storage_.objects[deathIndex];
        deathCold_.geometry120=&geometry_[deathIndex];deathCold_.globalValid3E0730=&storage_.globalValid3E0730;
    }
public:
    // Read-only identity of the existing immutable source constant; this grants
    // no access to a bridge instance, query state, storage or lifetime authority.
    static constexpr const SpringVector4_48N& sharedSentinel3E02C0()noexcept {
        return sentinel3E02C0;
    }
    // Recovery arguments are all-or-none. Its explicit stack capacity is the
    // cold maximum nodes+faces+1 across every selectable original podboxf mesh,
    // computed before Bank::seal; refresh rejects any selected mesh beyond it.
    // Existing Ground callers retain exactly the same owners and effects.
    QueryBridge(storage::Storage& track,const storage::Ground& ground,
        CollisionNormalModeOwner50JA& normalMode,Type2StaticQuery49X& main,CollisionFilter48J& mainFilter,
        Type2StaticQuery49X& secondary,CollisionFilter48J& secondaryFilter,TerrainQuery48J& terrainMetadata,
        undertrack_owners50ew_fixture::Owners* recovery=nullptr,const VehicleFrameState49W* frame=nullptr,
        Type3QueryInput48P* movement=nullptr,std::size_t recoveryMeshStackCapacity=0,
        std::size_t wreckMeshStackCapacity=0):
        QueryBridge(track,ground.world,normalMode,main,mainFilter,secondary,secondaryFilter,
            terrainMetadata,recovery,frame,movement,recoveryMeshStackCapacity,wreckMeshStackCapacity) {}
    QueryBridge(const QueryBridge&)=delete;QueryBridge& operator=(const QueryBridge&)=delete;
    QueryBridge(QueryBridge&&)=delete;QueryBridge& operator=(QueryBridge&&)=delete;
    const Diagnostic& diagnostic() const noexcept{return diagnostic_;}
    OriginalCameraQueryPairOwner& pairOwner() noexcept{return owner_;}
    bool prepared() const noexcept{return ready_;}
    const SpringVector4_48N& sharedZero433DB0() const noexcept{return zero433DB0;}
    bool bindDeathChild(std::uint64_t death68Key) noexcept {
        if(!ready_||closed_||synced_||!death68Key||deathOwner_.owner9C||deathOwner_.state)return false;
        deathOwner_.owner9C=death68Key;return true;
    }
    // Read-only borrow used by124CB0 reset. The active bank0 extent must be
    // established by this actual owner; opponent extents never call this API.
    bool ownsStorage(const storage::Storage& actual) const noexcept{return &actual==&storage_;}
    bool resetTailExcluded(const storage::Storage& actual,std::uint32_t floorMask,
        CollectionRef50BX& rejected) const noexcept {
        rejected=0;
        if(!ownsStorage(actual)||!ready_||closed_||synced_||!bindings()||
           storage_.collectionInput.objectCount!=storage_.count.objects||
           !floorMask||floorMask&(floorMask-1u)||
           owner_.world.objects!=storage_.objects.data()||
           owner_.mutableWorld.objects!=storage_.source.data()||
           owner_.sphereObject!=storage_.count.cameraSphere||owner_.rayObject!=storage_.count.cameraRay||
           owner_.sphereE0!=&sphereMembership_.state||owner_.rayE0!=&rayMembership_.state||
           !sphereMembership_.state.attached||!rayMembership_.state.attached||
           !admitOriginalCameraQueryPairStorage(owner_,nullptr,0,nullptr,0))return false;
        const auto& n=storage_.count;
        for(const auto ref:{n.cameraSphere,n.cameraRay}){
            rejected=ref;const auto& object=storage_.objects[ref-1u];
            if(object.identity!=(ref==n.cameraSphere?sphereIdentity:rayIdentity)||
               object.shapeTag!=(ref==n.cameraSphere?5u:3u))return false;
        }
        rejected=n.cameraDeath;
        if(deathOwner_.object120!=n.cameraDeath||deathOwner_.world.objects!=storage_.objects.data()||
           deathOwner_.mutableWorld.objects!=storage_.source.data()||
           deathOwner_.membershipE0!=&deathMembership_.state||
           deathCold_.object120!=&storage_.objects[n.cameraDeath-1u]||
           deathCold_.identity120!=deathIdentity||
           !admitOriginalDeathCameraQueryColdStorage(deathOwner_,deathCold_,nullptr,0,nullptr,0))return false;
        const auto& deathObject=storage_.objects[n.cameraDeath-1u];
        if(deathObject.identity!=deathIdentity||deathObject.hasShape!=(deathOwner_.state!=nullptr)||
           deathObject.shapeTag!=(deathOwner_.state?3u:0u))return false;
        if(deathOwner_.state&&(deathOwner_.state->objectIdentity120!=deathIdentity||
           deathOwner_.state->owner9C!=deathOwner_.owner9C||deathOwner_.state->callbackE4||
           deathObject.filter.categoryA4!=0u))return false;
        // The existing real piece owners prove every trailing E0/E4/E8 row.
        // No spare identity, fake geometry or reduced active extent is minted.
        rejected=n.wreckFirstQuery;
        if(!wreckRecordsReady())return false;
        for(auto ref=n.cameraSphere;ref<=n.objects;++ref){
            rejected=ref;const auto& object=storage_.objects[ref-1u];
            //124DF8 type and124E44 FLOOR gates precede124E84's F8 bit4 write.
            // An eligible tail requires its genuine reset shape dispatch;
            // this bounded borrow must reject it rather than silently skip it.
            if(!object.identity||(object.typeTag==main_.collisionTypeTag&&
                (object.filter.categoryA4&floorMask)))return false;
        }
        rejected=0;return true;
    }
    OriginalDeathCameraQueryOwner& deathOwner() noexcept{return deathOwner_;}
    OriginalDeathCameraQueryColdOwner& deathColdOwner() noexcept{return deathCold_;}

    // COLD only, before Follow prepare/Bank::seal. The actual decoded E4 mesh
    // must fit the explicit cold maximum supplied to this bridge constructor.
    // This returns borrows of the single shared world/staging/copy authority;
    // it does not construct the piece, write identities, register a query or
    // enlarge an active collector extent. The piece's source-order constructor
    // publishes those three rows; prepare() rejects an unfinished tail.
    bool bindWreckQueryWorldCold(std::size_t pieceIndex,OriginalWreckQueries50LA& queries,
        const OriginalWreckAssets50LA& assets,const std::array<std::uint32_t,3>& identities,
        const std::uint32_t& actualPhysicsBodyCategory3E0718,OriginalWreckQueryWorld50LA& out) noexcept {
        if(attempted_||ready_||closed_||synced_||pieceIndex>=wreck_.size()||wreck_[pieceIndex].owner||
           queries.prepared()||!assets.prepared()||!actualPhysicsBodyCategory3E0718||!bindings())return false;
        std::size_t extent=0;
        if(!meshExtent(assets.targetE4().mesh,extent)||extent>wreckMeshStackCapacity_||extent>meshStack_.size())return false;
        for(unsigned q=0;q<3;++q){
            const auto id=identities[q];
            if(!id||id==sphereIdentity||id==rayIdentity||id==deathIdentity)return false;
            for(unsigned previous=0;previous<q;++previous)if(id==identities[previous])return false;
            for(std::size_t j=0;j<storage_.count.objects;++j)if(storage_.objects[j].identity==id)return false;
            for(std::size_t j=0;j<wreck_.size();++j){
                if(wreck_[j].owner==&queries||wreck_[j].assets==&assets)return false;
                if(wreck_[j].owner)for(const auto previous:wreck_[j].identities)if(id==previous)return false;
            }
        }
        OriginalWreckQueryWorld50LA w{};
        w.lifecycle=&storage_.lifecycle;w.world=&storage_.world;w.workspace=&storage_.membershipWork;
        w.objects=storage_.objects.data();w.states=storage_.source.data();w.copies=copies_.data();
        w.geometry=geometry_.data();w.objectCount=storage_.count.objects;w.identities=identities;
        for(unsigned q=0;q<3;++q)w.refs[q]=static_cast<CollectionRef50BX>(storage_.count.wreckFirstQuery+pieceIndex*3u+q);
        w.physicsBodyCategory3E0718=&actualPhysicsBodyCategory3E0718;
        w.collisionTypeGp8010=&main_.collisionTypeTag;w.sentinel3E02C0=&sentinel3E02C0;
        w.globalValid3E0730=&storage_.globalValid3E0730;
        w.worldQuery=owner_.world;w.mutableQuery=owner_.mutableWorld;w.queryWorkspace=owner_.queryWorkspace;
        w.normalMode3DF5B0=owner_.normalMode3DF5B0;
        w.type2Targets=storage_.candidates.data();w.type2TargetCount=storage_.candidates.size();
        w.groupTypeGp8014=&main_.groupTypeTag;w.skipCategory3E0728=&main_.skipMask;
        wreck_[pieceIndex]={&queries,&assets,identities};out=w;return true;
    }


    // Join the accepted ordinary vehicle collector's F8 aliases. Call at its
    // existing successful publication point, before another query. The active
    // BANKED domain is checked first: opponent visits do not even read this
    // unbanked camera's state or memberships.
    bool publishCollection(const CollectionObjectState50BX* accepted,std::size_t activeCount) noexcept {
        const auto actualCount=storage_.collectionInput.objectCount;
        if(activeCount!=actualCount)return fail("Follow query collector extent differs",0x1223e4u);
        if(actualCount==nonCameraCount())return true;
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
        if(deathOwner_.state){const auto index=deathOwner_.object120-1u;
            storage_.source[index].flagsF8=accepted[index].flagsF8;
            deathOwner_.state->query120.flagsF8=storage_.source[index].flagsF8;
        }
        // Wreck F8 has no separate vehicle/camera projection. Publish the
        // accepted collector flags directly into its one canonical table.
        if(!wreckRecordsReady())return fail("Follow collector lost a concrete wreck owner",0x1223e4u);
        for(std::size_t i=0;i<storage_.count.wreckReferences;++i){
            const auto index=storage_.count.wreckFirstQuery-1u+i;
            storage_.source[index].flagsF8=accepted[index].flagsF8;
        }
        return true;
    }

    // Call only once, inside bank0, after the real Follow authoring produced
    // radius178 and after main690 completed its actual initial registration.
    bool prepare(float radius178) noexcept {
        if(attempted_||closed_)return fail("Follow query cold preparation cannot be replayed",0x179a80u);
        attempted_=true;
        if(storage_.collectionInput.objectCount!=nonCameraCount())
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
        storage_.globalValid3E0730=0; //Both actual11B570(2) calls clear the SAME world byte.
        const CollectionRef50BX refs[]{owner_.sphereObject,owner_.rayObject};
        for(unsigned i=0;i<2;++i){
            const auto index=refs[i]-1u;storage_.objects[index]=projection.objects[i];
            storage_.source[index]=projection.collection[i];copies_[index]=projection.copies[i];geometry_[index]={};
            geometry_[index].world.v[0]=geometry_[index].world.v[5]=
                geometry_[index].world.v[10]=geometry_[index].world.v[15]=1.f;
        }
        // This field is banked. Opponent domains retain main/secondary/terrain
        // plus the two actual recovery references when that route is enabled.
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
        // The shared mutable world is authoritative after any other query
        // visits these records. Pull both aliases; do not overwrite history.
        state_.sphere24.copied40=copies_[owner_.sphereObject-1u].copied40;
        state_.sphere24.copied50=copies_[owner_.sphereObject-1u].copied50;
        state_.ray190.copied40=copies_[owner_.rayObject-1u].copied40;
        state_.ray190.copied50=copies_[owner_.rayObject-1u].copied50;
        if(deathOwner_.state){const auto index=deathOwner_.object120-1u;auto& query=deathOwner_.state->query120;
            query.flagsF8=storage_.source[index].flagsF8;
            query.copied40=copies_[index].copied40;query.copied50=copies_[index].copied50;
        }
        synced_=true;diagnostic_.message="Follow query actual world synchronized";diagnostic_.sourcePC=0;return true;
    }
    bool commit() noexcept {
        if(!synced_||!ready_||closed_||storage_.collectionInput.objectCount!=storage_.count.objects)
            return fail("Follow query commit has no matching bank0 sync",0x121920u);
        if(!wreckRecordsReady())return fail("Follow query concrete wreck binding changed during lease",0x121b68u);
        for(std::size_t i=0;i<storage_.count.terrain;++i)
            if(!storage_.candidates[i]||storage_.objects[i].identity!=storage_.candidates[i]->candidate.collision.filter.identity)
                return fail("Follow query candidate owner changed during lease",0x121b68u);
        if(recovery_&&(!recoveryBindings()||
           storage_.objects[storage_.count.podQuery-1u].identity!=recovery_->podFilter.identity||
           storage_.objects[storage_.count.movementQuery-1u].identity!=movement_->filter.identity||
           geometry_[storage_.count.podQuery-1u].mesh!=&recovery_->selectedSource->mainTarget().mesh||
           std::memcmp(geometry_[storage_.count.podQuery-1u].world.v,frame_->shape.composed00.v,
               sizeof frame_->shape.composed00.v)))
            return fail("Follow query actual694/7A4 owner changed during lease",0x121b68u);
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
        if(recovery_){
            const auto pod=storage_.count.podQuery-1u,movement=storage_.count.movementQuery-1u;
            recovery_->podFilter.flagsF8=storage_.source[pod].flagsF8;
            movement_->filter.flagsF8=storage_.source[movement].flagsF8;
            commitCopies(pod,recovery_->podCopied40,recovery_->podCopied50);
            commitCopies(movement,movement_->copied40,movement_->copied50);
        }
        state_.sphere24.flagsF8=storage_.source[owner_.sphereObject-1u].flagsF8;
        state_.sphere24.copied40=copies_[owner_.sphereObject-1u].copied40;
        state_.sphere24.copied50=copies_[owner_.sphereObject-1u].copied50;
        state_.ray190.flagsF8=storage_.source[owner_.rayObject-1u].flagsF8;
        state_.ray190.copied40=copies_[owner_.rayObject-1u].copied40;
        state_.ray190.copied50=copies_[owner_.rayObject-1u].copied50;
        if(deathOwner_.state){const auto index=deathOwner_.object120-1u;auto& query=deathOwner_.state->query120;
            query.flagsF8=storage_.source[index].flagsF8;
            query.copied40=copies_[index].copied40;query.copied50=copies_[index].copied50;
        }
        synced_=false;diagnostic_.message="Camera query source-prefix writes committed to actual owners";
        diagnostic_.sourcePC=0;return true;
    }
    // Explicit teardown belongs inside bank0 while all external collision
    // owners still live. A failed removal retains its exact successful prefix;
    // retry continues with whichever real membership remains attached.
    bool close() noexcept {
        if(closed_)return true;
        // Actual22CE10 must remove each real wreck query before this host
        // bridge can shrink its active extent. Never orphan registered tail
        // references by hiding them behind a smaller objectCount.
        for(std::size_t i=0;i<wreck_.size();++i)if(wreck_[i].owner&&wreck_[i].owner->prepared())
            for(unsigned q=0;q<3;++q)if(wreck_[i].owner->query(q).membership.attached)
                return fail("Follow close still has a real registered wreck query",0x22ce10u);
        if((sphereMembership_.state.attached||rayMembership_.state.attached||deathMembership_.state.attached)&&
           storage_.collectionInput.objectCount!=storage_.count.objects)
            return fail("Follow query removal is outside bank0",0x120290u);
        if(synced_&&!commit())return false;
        for(auto* membership:{&sphereMembership_.state,&rayMembership_.state,&deathMembership_.state})if(membership->attached){
            diagnostic_.removal=removeLiveZoneMembership50EN(storage_.lifecycle,storage_.world,
                *membership,storage_.membershipWork);
            if(diagnostic_.removal.status!=LiveZoneStatus50EN::Complete)
                return fail("Follow query real120290 removal rejected",0x120290u);
        }
        storage_.collectionInput.objectCount=nonCameraCount();
        ready_=false;closed_=true;diagnostic_.message="Follow query memberships removed";
        diagnostic_.sourcePC=0;return true;
    }
};
} // namespace original_follow_query_bridge

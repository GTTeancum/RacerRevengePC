#pragma once
#include "track_runtime_storage50ig_fixture.h"
#include "undertrack_owners50ew_fixture.h"
#include "native_query_identities50kx.h"
#include "../../src/game/reset_ground_query50iz.h"
#include <algorithm>
#include <bit>
#include <cstring>
#include <limits>
#include <stdexcept>

// Concrete native124CB0 join for the original-AI physical reset. Construct
// fixed scratch cold, before Bank::seal. Query only inside the current racer's
// Bank::visit, after actual animation and registration publications, with no
// other collection open. This owner is unbanked scratch, not another world.
namespace original_ai_reset_ground_bridge {
using namespace rr::game;
namespace storage=track_runtime_storage50IG;
namespace recovery=undertrack_owners50ew_fixture;

struct Diagnostic {
    const char* message="AI reset ground bridge has not queried";
    ResetGroundDiagnostic50IZ query{};
};

class GroundBridge {
    storage::Storage& storage_;
    CollisionNormalModeOwner50JA& normalMode_;
    Type2StaticQuery49X& main_;
    CollisionFilter48J& mainFilter_;
    Type2StaticQuery49X& secondary_;
    CollisionFilter48J& secondaryFilter_;
    TerrainQuery48J& terrain_;
    VehicleFrameState49W& frame_;
    recovery::Owners& recovery_;
    Type3QueryInput48P& movement_;
    const std::uint32_t& floorMask_;
    storage::FixedBuffer<ResetGroundGeometry50IZ> geometry_;
    storage::FixedBuffer<CollectionProjection50CF> projections_;
    storage::FixedBuffer<CollectionObjectState50BX> scratchObjects_;
    storage::FixedBuffer<CollectionTreeState50BX> scratchMethods_;
    storage::FixedBuffer<std::uint32_t> meshStack_;
    // This is only the CollisionCandidate48I view of the SAME canonical694
    // shape and the selected pod's already-owned mesh. No mesh is copied.
    CollisionCandidate48I podGeometry_{};
    ResetGroundWorld50IZ world_{};
    ResetGroundMutable50IZ live_{};
    ResetGroundWorkspace50IZ work_{};
    Diagnostic diagnostic_{};
    //270A1C uses default0 for absent SceneInfo.UseEnvironmentNode. Admit only
    // that source route or explicit F; a T needs the real443108 owner/method.
    const CollectionRef50BX absentGlobalFallback443108_=0;

    static void admitNoEnvironmentFallback(const storage::Ground& ground) {
        const auto& world=ground.world;
        if(world.zones.empty()||world.scene_info_block>=world.parameters.blocks.size())
            throw std::runtime_error("AI reset original SceneInfo is unavailable");
        const auto& info=world.parameters.blocks[world.scene_info_block];
        if(info.name!="sceneinfo")throw std::runtime_error("AI reset SceneInfo identity differs");
        bool seen=false;
        for(const auto& field:info.fields)if(field.name=="useenvironmentnode"){
            if(seen||field.value!="F")
                throw std::runtime_error("AI reset requires the actual unsupported443108 environment owner");
            seen=true;
        }
    }
    static std::size_t meshCapacity(const storage::Storage& s) {
        std::size_t result=1;
        for(std::size_t i=0;i<s.count.terrain;++i){
            const auto* candidate=s.candidates[i];
            if(!candidate)throw std::runtime_error("AI reset actual candidate owner missing");
            const auto* mesh=candidate->candidate.collision.geometry.mesh;
            if(!mesh)continue;
            if(mesh->nodes.size()>=std::numeric_limits<std::size_t>::max()-mesh->faces.size())
                throw std::runtime_error("AI reset mesh-stack extent overflow");
            result=std::max(result,mesh->nodes.size()+mesh->faces.size()+1u);
        }
        // Actual694 is checked FLOOR-ineligible below. Its selected mesh can
        // change with the banked racer, but never consumes this query stack.
        return result;
    }
    bool fail(const char* text,std::uint32_t pc,CollectionRef50BX object=0) noexcept {
        diagnostic_.message=text;
        diagnostic_.query={ResetGroundStatus50IZ::Binding,pc,object,0,0,
            ResetTreeStatus50AK::Complete};
        return false;
    }
    bool layout() const noexcept {
        const auto& n=storage_.count;
        const bool camera=n.cameraSphere||n.cameraRay;
        return n.terrain&&n.recoveryReferences==2u&&
            n.mainQuery==n.terrain+1u&&n.secondaryQuery==n.terrain+2u&&n.terrainQuery==n.terrain+3u&&
            n.podQuery==n.terrain+4u&&n.movementQuery==n.terrain+5u&&
            (!camera||(n.cameraSphere==n.movementQuery+1u&&n.cameraRay==n.movementQuery+2u&&n.cameraDeath==n.movementQuery+3u))&&
            n.objects>=n.movementQuery+(camera?3u:0u)&&n.objects<=storage::runtimeSearchSlotLimit&&
            storage_.objects.size()==n.objects&&storage_.source.size()==n.objects&&
            storage_.registrations.size()==n.objects&&storage_.treeSource.size()==n.methods&&
            storage_.trees.size()==n.methods&&storage_.zones.size()==n.zones&&
            storage_.lifecycle.objectCount==n.objects&&storage_.lifecycle.zoneCount==n.zones&&
            storage_.lifecycle.storage.views.methods==storage_.trees.data()&&
            storage_.lifecycle.storage.views.zones==storage_.zones.data()&&
            storage_.lifecycle.storage.registrations==storage_.registrations.data()&&
            storage_.world.zoneCount==n.zones&&floorMask_&&!(floorMask_&(floorMask_-1u));
    }
    static bool empty(const ZoneRegistration50CJ& r) noexcept {
        return !r.owner&&!r.slot&&!r.encodedObject;
    }
    bool membership(const LiveZoneMembership50EN& m,CollectionRef50BX ref,
        const LiveZoneStorage50EN& backing,std::uint32_t shape) const noexcept {
        if(m.storage.links!=backing.links||m.storage.entries!=backing.entries||
           m.storage.capacity!=backing.capacity||!backing.links||!backing.entries||!backing.capacity||
           m.lastShapeTag!=shape)return false;
        if(!m.attached)return !m.object&&empty(storage_.registrations[ref-1u]);
        if(m.object!=ref||m.method!=ZoneMethod50CL::Second||!backing.entries[0].used)return false;
        const auto& primary=backing.entries[0].registration;
        const auto& actual=storage_.registrations[ref-1u];
        return primary.owner&&primary.owner==actual.owner&&primary.slot==actual.slot&&
            primary.encodedObject==actual.encodedObject&&primary.encodedObject==(ref<<2u);
    }
    bool spring(const Type2StaticQuery49X& query,const CollisionFilter48J& filter,
        storage::Membership& owner,CollectionRef50BX ref) const noexcept {
        const auto& m=owner.state;
        return query.filter==&filter&&query.resource&&m.lastType2Resource==query.resource&&
            membership(m,ref,owner.storage(),2u)&&
            !std::memcmp(query.worldMatrix.v,m.lastShapeWorld.v,sizeof query.worldMatrix.v)&&
            !std::memcmp(query.sphere.center.v,m.bounds.cachedCenter.v,sizeof query.sphere.center.v)&&
            !std::memcmp(&query.sphere.radius,&m.bounds.cachedRadius,sizeof query.sphere.radius);
    }
    bool stage(CollectionRef50BX ref,const CollisionFilter48J& filter,
        const CollisionSphere48J& sphere,std::uint8_t type,std::uint32_t shape,std::uint32_t w,
        const CollisionCandidate48I* geometry=nullptr) noexcept {
        auto& p=projections_[ref-1u];p=projectCollectionObject50CF(filter,sphere,type,shape,w);
        if(!p.valid)return fail("AI reset actual collision projection rejected",0x120e70u,ref);
        geometry_[ref-1u].shape0=geometry;return true;
    }
    bool stageMembership(CollectionRef50BX ref,const CollisionFilter48J& filter,
        std::uint8_t type,const LiveZoneMembership50EN& m,std::uint32_t shape) noexcept {
        CollisionSphere48J sphere{};std::memcpy(sphere.center.v,m.bounds.cachedCenter.v,sizeof sphere.center.v);
        sphere.radius=m.bounds.cachedRadius;
        return stage(ref,filter,sphere,type,shape,std::bit_cast<std::uint32_t>(m.bounds.cachedCenter.v[3]));
    }
    bool refreshRecords() noexcept {
        const auto& n=storage_.count;
        const auto active=storage_.collectionInput.objectCount;
        if(!layout()||storage_.queue.count||(active!=n.movementQuery&&active!=n.objects)||
           !recovery_.storagePrepared||!recovery_.sourcePrepared||!recovery_.selectedSource||
           !recovery_.selectedSource->prepared()||
           main_.resource!=&recovery_.selectedSource->main()||
           secondary_.resource!=&recovery_.selectedSource->secondary()||
           main_.collisionTypeTag!=secondary_.collisionTypeTag||main_.collisionTypeTag!=terrain_.collisionTypeTag||
           main_.collisionTypeTag!=movement_.collisionTypeTag||main_.collisionTypeTag!=recovery_.globals.collisionTypeTag||
           recovery_.podFilter.identity!=recovery_.globals.identity[0]||
           movement_.filter.identity!=recovery_.globals.identity[1]||recovery_.movementTag!=3u||
           frame_.shape.type88!=0u||(recovery_.podFilter.categoryA4&floorMask_)||
           !spring(main_,mainFilter_,storage_.mainMembership,n.mainQuery)||
           !spring(secondary_,secondaryFilter_,storage_.secondaryMembership,n.secondaryQuery)||
           !membership(storage_.terrainMembership.state,n.terrainQuery,storage_.terrainMembership.storage(),2u)||
           !membership(recovery_.memberships[0],n.podQuery,recovery_.storage(0),0u)||
           !membership(recovery_.memberships[1],n.movementQuery,recovery_.storage(1),3u)||
           frame_.shape.registrationE0!=(recovery_.memberships[0].attached?n.podQuery:0u))
            return fail("AI reset world, actual query owner, or registration publication differs",0x216ea8u);
        for(std::size_t i=0;i<n.terrain;++i){
            const auto* candidate=storage_.candidates[i];
            if(!candidate)return fail("AI reset current concrete candidate missing",0x124df8u,static_cast<CollectionRef50BX>(i+1u));
            const auto& actual=candidate->candidate.collision;
            if(!actual.filter.identity||actual.geometry.id!=actual.filter.identity||
               (candidate->shapeType==0u&&!actual.geometry.mesh))
                return fail("AI reset actual candidate identity or shape0 geometry differs",0x12482cu,static_cast<CollectionRef50BX>(i+1u));
            if(!stage(static_cast<CollectionRef50BX>(i+1u),actual.filter,actual.sphere,actual.typeTag,
                candidate->shapeType,std::bit_cast<std::uint32_t>(storage_.source[i].cachedCenter.v[3]),
                candidate->shapeType==0u?&actual.geometry:nullptr))return false;
        }
        if(!stageMembership(n.mainQuery,mainFilter_,main_.collisionTypeTag,storage_.mainMembership.state,2u)||
           !stageMembership(n.secondaryQuery,secondaryFilter_,secondary_.collisionTypeTag,storage_.secondaryMembership.state,2u)||
           !stageMembership(n.terrainQuery,terrain_.filter,terrain_.collisionTypeTag,storage_.terrainMembership.state,2u))return false;
        CollisionSphere48J podSphere{};std::memcpy(podSphere.center.v,frame_.shape.center60.v,sizeof podSphere.center.v);
        podSphere.radius=frame_.shape.radius80;
        podGeometry_={&recovery_.selectedSource->mainTarget().mesh,frame_.shape.composed00,recovery_.podFilter.identity};
        if(!stage(n.podQuery,recovery_.podFilter,podSphere,recovery_.globals.collisionTypeTag,0u,
                std::bit_cast<std::uint32_t>(frame_.shape.center60.v[3]),&podGeometry_)||
           !stage(n.movementQuery,movement_.filter,recovery_.movementSphere,movement_.collisionTypeTag,
                recovery_.movementTag,std::bit_cast<std::uint32_t>(recovery_.memberships[1].bounds.cachedCenter.v[3])))return false;
        if(n.cameraSphere&&active==n.objects)for(const auto ref:{n.cameraSphere,n.cameraRay}){
            const auto i=ref-1u;const auto& actual=storage_.objects[i];geometry_[i]={};
            // Camera rows are already the actual camera's source projection.
            //124CB0 has NO F8 target-enable test. Preserve all camera owners by
            // proving type/category exclusion before even the F8 bit4 write.
            if(actual.identity!=(ref==n.cameraSphere?native_query_identities50KX::followSphere24:
                    native_query_identities50KX::followRay190)||
               actual.shapeTag!=(ref==n.cameraSphere?5u:3u)||
               (actual.typeTag==main_.collisionTypeTag&&(actual.filter.categoryA4&floorMask_)))
                return fail("AI reset camera source projection could be mutated",0x124e44u,ref);
        }
        // Validate identity provenance before publishing any refreshed records.
        for(std::size_t i=0;i<active;++i){
            const bool camera=i+1u==n.cameraSphere||i+1u==n.cameraRay;
            const auto id=camera?storage_.objects[i].identity:projections_[i].object.identity;
            if(!id)return fail("AI reset actual object identity is unbound",0x124df8u,static_cast<CollectionRef50BX>(i+1u));
            for(std::size_t j=0;j<i;++j){
                const bool earlierCamera=j+1u==n.cameraSphere||j+1u==n.cameraRay;
                const auto other=earlierCamera?storage_.objects[j].identity:projections_[j].object.identity;
                if(id==other)return fail("AI reset actual collision identities alias",0x124df8u,static_cast<CollectionRef50BX>(i+1u));
            }
        }
        for(std::size_t i=0;i<active;++i){
            if(i+1u==n.cameraSphere||i+1u==n.cameraRay)continue;
            const auto& projection=projections_[i];storage_.objects[i]=projection.object;
            // Synchronize actual cache/filter lanes only. Epochs, group-member
            // state and every method field remain their current source owners.
            auto& source=storage_.source[i];source.flagsF8=projection.state.flagsF8;
            source.cachedCenter=projection.state.cachedCenter;source.cachedRadius=projection.state.cachedRadius;
        }
        // The current bank's entire concrete namespace is contiguous. Camera
        // owners are a trailing pair only in their actual bank0 arena; inactive
        // tail storage is never relabeled as a fake object or recovery owner.
        world_.objectCount=active;
        return true;
    }
    void publishAcceptedFlags() noexcept {
        const auto& n=storage_.count;
        for(std::size_t i=0;i<n.terrain;++i){
            const auto flags=storage_.source[i].flagsF8;
            storage_.candidates[i]->candidate.collision.filter.flagsF8=flags;
            storage_.movement[i].collision.filter.flagsF8=flags;
        }
        mainFilter_.flagsF8=storage_.source[n.mainQuery-1u].flagsF8;
        secondaryFilter_.flagsF8=storage_.source[n.secondaryQuery-1u].flagsF8;
        terrain_.filter.flagsF8=storage_.source[n.terrainQuery-1u].flagsF8;
        recovery_.podFilter.flagsF8=storage_.source[n.podQuery-1u].flagsF8;
        movement_.filter.flagsF8=storage_.source[n.movementQuery-1u].flagsF8;
        // treeSource is passed directly to124CB0, so accepted SearchList
        // cursor/flags16 writes already reside in the actual method owners.
    }
public:
    GroundBridge(storage::Storage& track,const storage::Ground& ground,
        CollisionNormalModeOwner50JA& normalMode,Type2StaticQuery49X& main690,CollisionFilter48J& mainFilter,
        Type2StaticQuery49X& secondary,CollisionFilter48J& secondaryFilter,TerrainQuery48J& terrain348,
        VehicleFrameState49W& canonicalFrame,recovery::Owners& recoveryOwners,
        Type3QueryInput48P& actualMovement7A4,const std::uint32_t& floorMask):
        storage_(track),normalMode_(normalMode),main_(main690),mainFilter_(mainFilter),secondary_(secondary),
        secondaryFilter_(secondaryFilter),terrain_(terrain348),frame_(canonicalFrame),recovery_(recoveryOwners),
        movement_(actualMovement7A4),floorMask_(floorMask),geometry_(track.count.objects),
        projections_(track.count.objects),scratchObjects_(track.count.objects),scratchMethods_(track.count.methods),
        meshStack_(meshCapacity(track)) {
        admitNoEnvironmentFallback(ground);
        if(!layout())throw std::runtime_error("AI reset bridge requires the actual reserved track-storage namespace");
        world_.objects=track.objects.data();world_.geometry=geometry_.data();world_.objectCount=track.count.objects;
        world_.methods=track.trees.data();world_.methodCount=track.count.methods;
        world_.zones=track.zones.data();world_.zoneCount=track.count.zones;
        live_={track.source.data(),track.count.objects,track.treeSource.data(),track.count.methods};
        work_={scratchObjects_.data(),scratchObjects_.size(),scratchMethods_.data(),scratchMethods_.size(),
            meshStack_.data(),meshStack_.size()};
    }
    GroundBridge(const GroundBridge&)=delete;GroundBridge& operator=(const GroundBridge&)=delete;
    GroundBridge(GroundBridge&&)=delete;GroundBridge& operator=(GroundBridge&&)=delete;
    const Diagnostic& diagnostic() const noexcept{return diagnostic_;}

    ResetGroundInput50IZ actualResetGroundInput50IZ(CollectionRef50BX actualC34,
        const SpringVector4_48N& start,const SpringVector4_48N& end) const noexcept {
        ResetGroundInput50IZ input{};input.start=start;input.end=end;input.zoneC34=actualC34;
        input.globalFallbackZone443108=&absentGlobalFallback443108_;input.floorMask=floorMask_;
        input.directTypeTagGp8010=&main_.collisionTypeTag;input.normalMode3DF5B0=normalMode_.boundWord();
        return input;
    }
    // One actual216EA8/216F04 request per call. The core's source probe owner
    // decides whether to retry, up to its actual20-attempt limit. Initialize
    // out.point/out.normal from source VF0=(0,0,0,1) once at216DE0, then retain
    // them across calls. A genuine miss preserves those exact prior words.
    ResetGroundStatus50IZ query(CollectionRef50BX actualC34,const SpringVector4_48N& start,
        const SpringVector4_48N& end,ResetGroundResult50IZ& out) noexcept {
        diagnostic_={};
        if(!normalMode_.boundWord()){
            fail("AI reset actual application collision normal mode is not yet bound",0x124cb0u);
            return diagnostic_.query.status;
        }
        if(!actualC34||actualC34>storage_.count.zones){
            fail("AI reset actualC34 is outside the actual zone namespace",0x216e84u);
            return diagnostic_.query.status;
        }
        if(!refreshRecords())return diagnostic_.query.status;
        const auto input=actualResetGroundInput50IZ(actualC34,start,end);
        const auto status=queryResetGround50IZ(input,world_,live_,work_,out,diagnostic_.query);
        if(status!=ResetGroundStatus50IZ::Complete){
            diagnostic_.message="AI reset124CB0 rejected; no miss or completed source call was fabricated";
            return status;
        }
        publishAcceptedFlags();
        diagnostic_.message=out.hit?"AI reset actual124CB0 hit committed":"AI reset actual124CB0 miss committed";
        return status;
    }
};
} // namespace original_ai_reset_ground_bridge

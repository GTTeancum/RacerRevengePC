#pragma once
#include "track_runtime_storage50ig_fixture.h"
#include "undertrack_owners50ew_fixture.h"
#include "native_query_identities50kx.h"
#include "original_follow_query_bridge50kx.h"
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
    bool sourceQueryActive_=false;
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
    const original_follow_query_bridge::QueryBridge* canonicalTailOwner_=nullptr;
    //270A1C uses default0 for absent SceneInfo.UseEnvironmentNode. Admit only
    // that source route or explicit F; a T needs the real443108 owner/method.
    const CollectionRef50BX absentGlobalFallback443108_=0;

    static void admitNoEnvironmentFallback(const rr::assets::World48F& world) {
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
        const storage::RosterRowRef& row) const noexcept {
        if(!row)return false;
        const auto& m=*row.state;
        return query.filter==&filter&&query.resource&&m.lastType2Resource==query.resource&&
            membership(m,row.ref,row.storage,2u)&&
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
        //The AI slot's own five rows through the one storage accessor (current
        //banked racer; this callback runs inside that AI's visit).
        const auto mainRow=storage_.currentRow(storage::RosterRole::main690);
        const auto secondaryRow=storage_.currentRow(storage::RosterRole::secondary364);
        const auto terrainRow=storage_.currentRow(storage::RosterRole::terrain348);
        const auto podRow=storage_.currentRow(storage::RosterRole::pod694);
        const auto movementRow=storage_.currentRow(storage::RosterRole::movement7A4);
        if(!layout()||storage_.queue.count||(active!=n.movementQuery&&active!=n.objects)||
           !mainRow||!secondaryRow||!terrainRow||!podRow||!movementRow||
           !recovery_.storagePrepared||!recovery_.sourcePrepared||!recovery_.selectedSource||
           !recovery_.selectedSource->prepared()||
           main_.resource!=&recovery_.selectedSource->main()||
           secondary_.resource!=&recovery_.selectedSource->secondary()||
           main_.collisionTypeTag!=secondary_.collisionTypeTag||main_.collisionTypeTag!=terrain_.collisionTypeTag||
           main_.collisionTypeTag!=movement_.collisionTypeTag||main_.collisionTypeTag!=recovery_.globals.collisionTypeTag||
           recovery_.podFilter.identity!=recovery_.globals.identity[0]||
           movement_.filter.identity!=recovery_.globals.identity[1]||recovery_.movementTag!=3u||
           frame_.shape.type88!=0u||(recovery_.podFilter.categoryA4&floorMask_)||
           !spring(main_,mainFilter_,mainRow)||
           !spring(secondary_,secondaryFilter_,secondaryRow)||
           !membership(*terrainRow.state,terrainRow.ref,terrainRow.storage,2u)||
           !membership(*podRow.state,podRow.ref,podRow.storage,0u)||
           !membership(*movementRow.state,movementRow.ref,movementRow.storage,3u)||
           frame_.shape.registrationE0!=(podRow.state->attached?podRow.ref:0u))
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
        if(!stageMembership(mainRow.ref,mainFilter_,main_.collisionTypeTag,*mainRow.state,2u)||
           !stageMembership(secondaryRow.ref,secondaryFilter_,secondary_.collisionTypeTag,*secondaryRow.state,2u)||
           !stageMembership(terrainRow.ref,terrain_.filter,terrain_.collisionTypeTag,*terrainRow.state,2u))return false;
        CollisionSphere48J podSphere{};std::memcpy(podSphere.center.v,frame_.shape.center60.v,sizeof podSphere.center.v);
        podSphere.radius=frame_.shape.radius80;
        podGeometry_={&recovery_.selectedSource->mainTarget().mesh,frame_.shape.composed00,recovery_.podFilter.identity};
        if(!stage(podRow.ref,recovery_.podFilter,podSphere,recovery_.globals.collisionTypeTag,0u,
                std::bit_cast<std::uint32_t>(frame_.shape.center60.v[3]),&podGeometry_)||
           !stage(movementRow.ref,movement_.filter,recovery_.movementSphere,movement_.collisionTypeTag,
                recovery_.movementTag,std::bit_cast<std::uint32_t>(movementRow.state->bounds.cachedCenter.v[3])))return false;
        const bool canonicalTail=n.cameraSphere&&active==n.objects;
        if(canonicalTail){
            CollectionRef50BX rejected=0;
            if(!canonicalTailOwner_||!canonicalTailOwner_->resetTailExcluded(storage_,floorMask_,rejected))
                return fail("AI reset actual camera/wreck tail ownership or FLOOR exclusion rejected",0x124e44u,rejected);
            for(auto ref=n.cameraSphere;ref<=n.objects;++ref)geometry_[ref-1u]={};
        }
        const auto preserved=[&](std::size_t i) noexcept {
            return canonicalTail&&i+1u>=n.cameraSphere;
        };
        // Validate identity provenance before publishing any refreshed records.
        for(std::size_t i=0;i<active;++i){
            const bool camera=preserved(i);
            const auto id=camera?storage_.objects[i].identity:projections_[i].object.identity;
            if(!id)return fail("AI reset actual object identity is unbound",0x124df8u,static_cast<CollectionRef50BX>(i+1u));
            for(std::size_t j=0;j<i;++j){
                const bool earlierCamera=preserved(j);
                const auto other=earlierCamera?storage_.objects[j].identity:projections_[j].object.identity;
                if(id==other)return fail("AI reset actual collision identities alias",0x124df8u,static_cast<CollectionRef50BX>(i+1u));
            }
        }
        for(std::size_t i=0;i<active;++i){
            if(preserved(i))continue;
            const auto& projection=projections_[i];storage_.objects[i]=projection.object;
            // Synchronize actual cache/filter lanes only. Epochs, group-member
            // state and every method field remain their current source owners.
            auto& source=storage_.source[i];source.flagsF8=projection.state.flagsF8;
            source.cachedCenter=projection.state.cachedCenter;source.cachedRadius=projection.state.cachedRadius;
        }
        // The current bank's entire concrete namespace is contiguous. Camera
        // and constructed wreck owners form the actual bank0 tail; inactive
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
        //Current racer's rows; an unresolvable row is refused (lane left as is),
        //never indexed. refreshRecords already required all five this call.
        const auto copy=[&](storage::RosterRole role,std::uint8_t& lane) noexcept {
            const auto row=storage_.currentRow(role);if(row)lane=storage_.source[row.ref-1u].flagsF8;};
        copy(storage::RosterRole::main690,mainFilter_.flagsF8);
        copy(storage::RosterRole::secondary364,secondaryFilter_.flagsF8);
        copy(storage::RosterRole::terrain348,terrain_.filter.flagsF8);
        copy(storage::RosterRole::pod694,recovery_.podFilter.flagsF8);
        copy(storage::RosterRole::movement7A4,movement_.filter.flagsF8);
        // treeSource is passed directly to124CB0, so accepted SearchList
        // cursor/flags16 writes already reside in the actual method owners.
    }
public:
    GroundBridge(storage::Storage& track,const rr::assets::World48F& sourceWorld,
        CollisionNormalModeOwner50JA& normalMode,Type2StaticQuery49X& main690,CollisionFilter48J& mainFilter,
        Type2StaticQuery49X& secondary,CollisionFilter48J& secondaryFilter,TerrainQuery48J& terrain348,
        VehicleFrameState49W& canonicalFrame,recovery::Owners& recoveryOwners,
        Type3QueryInput48P& actualMovement7A4,const std::uint32_t& floorMask):
        storage_(track),normalMode_(normalMode),main_(main690),mainFilter_(mainFilter),secondary_(secondary),
        secondaryFilter_(secondaryFilter),terrain_(terrain348),frame_(canonicalFrame),recovery_(recoveryOwners),
        movement_(actualMovement7A4),floorMask_(floorMask),geometry_(track.count.objects),
        projections_(track.count.objects),scratchObjects_(track.count.objects),scratchMethods_(track.count.methods),
        meshStack_(meshCapacity(track)) {
        admitNoEnvironmentFallback(sourceWorld);
        if(!layout())throw std::runtime_error("AI reset bridge requires the actual reserved track-storage namespace");
        world_.objects=track.objects.data();world_.geometry=geometry_.data();world_.objectCount=track.count.objects;
        world_.methods=track.trees.data();world_.methodCount=track.count.methods;
        world_.zones=track.zones.data();world_.zoneCount=track.count.zones;
        live_={track.source.data(),track.count.objects,track.treeSource.data(),track.count.methods};
        work_={scratchObjects_.data(),scratchObjects_.size(),scratchMethods_.data(),scratchMethods_.size(),
            meshStack_.data(),meshStack_.size()};
    }
    GroundBridge(storage::Storage& track,const storage::Ground& ground,
        CollisionNormalModeOwner50JA& normalMode,Type2StaticQuery49X& main690,CollisionFilter48J& mainFilter,
        Type2StaticQuery49X& secondary,CollisionFilter48J& secondaryFilter,TerrainQuery48J& terrain348,
        VehicleFrameState49W& canonicalFrame,recovery::Owners& recoveryOwners,
        Type3QueryInput48P& actualMovement7A4,const std::uint32_t& floorMask):
        GroundBridge(track,ground.world,normalMode,main690,mainFilter,secondary,secondaryFilter,
            terrain348,canonicalFrame,recoveryOwners,actualMovement7A4,floorMask){}
    GroundBridge(const GroundBridge&)=delete;GroundBridge& operator=(const GroundBridge&)=delete;
    GroundBridge(GroundBridge&&)=delete;GroundBridge& operator=(GroundBridge&&)=delete;
    const Diagnostic& diagnostic() const noexcept{return diagnostic_;}
    // Cold binding after the actual Follow owner exists, before Bank::seal.
    bool bindCanonicalTailOwner(const original_follow_query_bridge::QueryBridge& owner) noexcept {
        if(canonicalTailOwner_||!owner.ownsStorage(storage_))return false;
        canonicalTailOwner_=&owner;return true;
    }


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
    ResetGroundStatus50IZ querySourceBound50MB(CollectionRef50BX actualC34,const SpringVector4_48N& start,
        const SpringVector4_48N& end,ResetGroundResult50IZ& out,
        CollisionResultBindingOwner50MB& binding,const SourceOuterResultLease50MB& parent) noexcept {
        if(sourceQueryActive_||!binding.outerCurrent(parent)||!layout())return ResetGroundStatus50IZ::Binding;
        // Validate every actual bridge write BEFORE even the reentry flag or
        // diagnostic changes. Shared canonical F8 writes remain intentional;
        // they must still be disjoint from protected result/scratch/list data.
        const auto separate=[&](const auto* data,std::size_t count) noexcept {
            return count<=std::numeric_limits<std::size_t>::max()/sizeof(*data)&&
                binding.disjointOuterStorage(data,count*sizeof(*data));
        };
        if(!separate(this,1)||!separate(&binding,1)||!separate(&parent,1)||!separate(&out,1)||
           !separate(geometry_.data(),geometry_.size())||!separate(projections_.data(),projections_.size())||
           !separate(scratchObjects_.data(),scratchObjects_.size())||!separate(scratchMethods_.data(),scratchMethods_.size())||
           !separate(meshStack_.data(),meshStack_.size())||!separate(storage_.objects.data(),storage_.objects.size())||
           !separate(storage_.source.data(),storage_.source.size())||!separate(storage_.treeSource.data(),storage_.treeSource.size())||
           !separate(&mainFilter_.flagsF8,1)||!separate(&secondaryFilter_.flagsF8,1)||
           !separate(&terrain_.filter.flagsF8,1)||!separate(&recovery_.podFilter.flagsF8,1)||
           !separate(&movement_.filter.flagsF8,1))return ResetGroundStatus50IZ::Storage;
        for(std::size_t i=0;i<storage_.count.terrain;++i){
            if(!storage_.candidates[i])return ResetGroundStatus50IZ::Binding;
            if(!separate(&storage_.candidates[i]->candidate.collision.filter.flagsF8,1)||
               !separate(&storage_.movement[i].collision.filter.flagsF8,1))return ResetGroundStatus50IZ::Storage;
        }
        struct Guard {bool& flag;~Guard(){flag=false;}} guard{sourceQueryActive_};sourceQueryActive_=true;
        diagnostic_={};
        if(!normalMode_.boundWord()||!actualC34||actualC34>storage_.count.zones){
            fail("source-trigger ground owner/mode binding rejected",0x124cb0u);return diagnostic_.query.status;
        }
        if(!refreshRecords())return diagnostic_.query.status;
        const auto input=actualResetGroundInput50IZ(actualC34,start,end);
        const auto status=queryResetGroundSourceBound50MB(input,world_,live_,work_,out,diagnostic_.query,binding,parent);
        if(status==ResetGroundStatus50IZ::Complete)publishAcceptedFlags();
        diagnostic_.message=status==ResetGroundStatus50IZ::Complete?
            "source-trigger ground call completed with scoped native result lifetime":"source-trigger ground call blocked";
        return status;
    }

};
} // namespace original_ai_reset_ground_bridge

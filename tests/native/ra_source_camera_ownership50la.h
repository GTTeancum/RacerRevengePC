#pragma once
#include "ra_cold_roster_resources50la.h"
#include "native_racer_cables50hw_fixture.h"
namespace original_main_entry50KG {class NativeCollectorFifo50LA;}
namespace original_cable_selection50LA { class Binding; }
namespace rr::game { class OriginalRaceConstructor50LA; }
#include "ra_static_candidates50la.h"
#include "native_racer_bank50hn_fixture.h"
#include "native_dynamic_contact_peers50iw_fixture.h"
#include "ra_source_camera_inputs50la.h"
#include "ra_track_fog50la.h"
#include "ra_static_fog_observation50la.h"
#include "original_follow_query_bridge50kx.h"
#include "terrain_probe_authoring50fq_fixture.h"
#include "original_racer_terrain_query_owner50la.h"
#include "original_racer_query690_owner50la.h"
#include "original_racer_secondary_body_owner50la.h"
#include "native_racer_grid50hn_fixture.h"
#include "source_grid50ig_fixture.h"
#include "../../src/game/query_prepare48o.h"
#include "../../src/game/reset_visibility50ey.h"
#include "../../src/game/indexed_type2_contact50cc.h"
#include "../../src/game/vehicle_contact48r.h"
#include "../../src/game/chariot_scene49u.h"
#include "../../src/game/spring_force48l.h"
#include "../../src/game/boost_controls50fk.h"
#include "../../src/game/type0_bounds50ew.h"
#include "../../src/game/collision_result_binding50mb.h"
#include "../../src/game/collection_projection50cf.h"
#include "../../src/game/pod_live_query50iz.h"
#include "../../src/game/original_racer_selection50ij.h"
#include "../../src/game/original_racer_gravity_owner50la.h"
#include "../../src/game/original_pod_sphere_owner50la.h"
#include "../../src/game/original_racer_model_scene50la.h"
#include "../../src/game/boost_controls50fk.h"
#include <bit>
#include <functional>
#include <string_view>
#include <cstddef>
#include <algorithm>
#include <exception>
#include <memory>
#include <limits>
#include <vector>

namespace interactive_setup50DU { struct Context; }
namespace ra_cold_aggregate50LA { class Owner; }
namespace ra_pod_scene50LA { class Owner; }
namespace native_racer_model50LA { class Owner; }
namespace ra_cold_roster50LA { class Resources; class Operation; class FreshExecutionArena; }
namespace ra_static_lighting50LA { class Binding; }
namespace ra_static_fog50LA { class Binding; }
#include "original_pod_load50pl.h"
namespace original_pod_load50PL { class SceneTrackData50PL; }
namespace ra_source_camera50LA {
namespace g=rr::game;
namespace storage=track_runtime_storage50IG;
// Actual objects/immutable assets, not counts or claimed query readiness.
// These borrows remain stable until this route has retired and been destroyed.
struct WreckReservation {
    g::OriginalWreckQueries50LA* queries=nullptr;
    const g::OriginalWreckAssets50LA* assets=nullptr;
    std::array<std::uint32_t,3> identities{};
};
struct RuntimeQueryCapacityInputs {
    std::vector<const native_racer_collision50HW::Owner*> collisionRoster;
    // Selects the existing actual694/7A4 route. Storage may still be unprepared:
    // its true membership capacity is derived by this world's construction.
    undertrack_owners50ew_fixture::Owners* recovery=nullptr;
    std::vector<WreckReservation> wreck;
    std::uint32_t humanCount=0;
};
struct ActualQueryOwners {
    g::VehicleFrameState49W& frame;
    g::CollisionNormalModeOwner50JA& normalMode;
    g::Type2StaticQuery49X& main;
    g::CollisionFilter48J& mainFilter;
    g::Type2StaticQuery49X& secondary;
    g::CollisionFilter48J& secondaryFilter;
    const native_racer_collision50HW::Owner& selectedCollision;
    terrain_probe_authoring50fq_fixture::Owner& terrain;
    live_terrain_probe50fr::Owner& terrainLive;
    // Must be the same recovery owner selected before Storage construction.
    undertrack_owners50ew_fixture::Owners* recovery=nullptr;
    g::Type3QueryInput48P* movement=nullptr;
};
// Stable actual arena fields already registered by the real constructor caller.
// The bridge supplies frame/query/world/source owners internally. No pose,
// world, rank, completion, scratch pointer or generic callback is accepted.
struct GridArenaOwners {
    g::VehicleFrameBindings49W& bindings;
    g::VehicleFrameParameters49W& parameters;
    g::SubstepSelectionContext49K& selectors; //Actual registered per-racer Fixture lane, not a copied/default37C.
    g::AiRuntimeState50IV& ai;
    g::BasePoseHistoryState50Y& history;
    const std::array<std::unique_ptr<native_racer50HW::Rig>,8>& rigs;
    std::uint32_t humanCount=0;
};
struct FreshRecoveryArenaOwners {
    g::SpringVector4_48N (&endpoints)[2];
    std::uint32_t (&metadata)[2];
    const g::Type3QuerySource48P& source;
};
struct Diagnostic {
    const char* message=nullptr;
    storage::Diagnostic storage;
    original_follow_query_bridge::Diagnostic follow;
    original_camera_session50KX::Result camera{};
};
// Bounded fresh runtime backing prefix. It owns the sole RA registry backing
// and Follow/death cold buffers while borrowing the existing body/query arena.
// No public constructor, mutable Storage, query getter, attach or camera API.
class OwnershipBridge final {
    friend class rr::game::OriginalRaceConstructor50LA; //Exact owned Camera34 native adapter identity only.
    friend struct ::interactive_setup50DU::Context;
    class SharedTerrainInvocation;
    struct EndCandidateResult {
        std::uint32_t stoppedPC=0x219ad4;
        std::uint64_t projectionRevision=0;
        std::int32_t firstCount=0,secondCount=0,mergedCount=0;
        bool firstReturned=false,secondReturned=false,merged=false;
        bool sharedFirstFlagsPublished=false,sharedSecondFlagsPublished=false;
        bool nativeDetectorEntered=false,nativeDetectorReset=false,nativeDetectorReturned=false;
        bool nativeQueryCachePrepared=false,nativeQueryMembershipCommitted=false,nativeQueryPrefixReturned=false;
        std::uint64_t nativeQueryMembershipRevision=0;
        bool nativeLimitedInitialized=false,nativeLimitedReturned=false;
        std::size_t nativeLimitedOrdinal=0;
        bool nativeSecondaryBodyBound=false,nativeSecondarySkipped=false;
        bool nativeSecondaryDetectorEntered=false,nativeSecondaryDetectorReset=false,nativeSecondaryDetectorReturned=false;
        bool nativeSecondaryCachePrepared=false,nativeSecondaryMembershipCommitted=false,nativeSecondaryQueryPrefixReturned=false;
        std::uint64_t nativeSecondaryMembershipRevision=0;
        bool nativeSecondaryLimitedInitialized=false,nativeSecondaryLimitedReturned=false;
        std::size_t nativeSecondaryLimitedOrdinal=0;
        bool nativeContactStaticsEntered=false,nativeContactScenePrepared=false,nativeContactReturned=false;
        bool nativeTerrainEntered=false,nativeTerrainSeedPublished=false;
        std::array<std::uint32_t,3> nativeContactPosition{},nativeContactForward{},nativeContactUp{};
        std::size_t nativeContactOrdinal=0;
        std::uint32_t nativeContactMatchedIdentity=0;
    };
    //Immutable policy membership producer is installed at fresh Bank
    //registration. Select only the actual active slot; never mutate Entry.
    bool selectNativeDetectorMembership(std::size_t slot,bool secondary,
        g::LiveZoneMembership50EN*& member,std::uint32_t& ref,
        storage::Storage::SharedRosterObject*& row)noexcept {
        auto* s=storage_.get();auto* policy=s?s->nativeDetectorPolicy_.get():nullptr;
        if(!s||!policy||!policy->live_||!policy->registered_)return false;
        const auto lane=secondary?1u:0u;
        if(!s->sharedRoster_){
            if(policy->sharedWorld_||policy->sharedRetired_)return false;
            member=secondary?&s->secondaryMembership.state:&s->mainMembership.state;
            ref=secondary?s->count.secondaryQuery:s->count.mainQuery;row=nullptr;
            return policy->entries_[lane].membership==member&&policy->entries_[lane].ref==ref;
        }
        const auto pin=bankLifetime_.lock();const auto* producer=endCandidateProducer_;
        if(policy->sharedWorld_!=s||policy->sharedRetired_||!pin||!bankIdentity_||
           pin->identity!=bankIdentity_||!producer||!producer->inCall||!producer->activeSession||
           producer->activeSlot!=slot||&producer->activeSession->actual_.bank!=bankIdentity_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->visitDepth_!=1u||
           bankIdentity_->currentSlot_!=slot||slot>=policy->sharedCount_||
           policy->sharedCount_!=s->sharedRoster_->count||s->sharedRoster_->retired||sharedEffectsStopped_)return false;
        const auto& owned=s->sharedRoster_->objects[slot*storage::Storage::rosterRoles+lane];
        const auto& selected=policy->sharedMemberships_[slot][lane];
        if(!owned||!owned->coldBound||!owned->coldFilter||owned->slot!=slot||
           static_cast<unsigned>(owned->role)!=lane||owned->generation!=producer->activeSession->generation_||
           selected.owner!=&owned->membership.state||selected.ref!=owned->reservedRef||
           !owned->reservedRef||owned->reservedRef>s->count.objects||
           owned->canonicalFilter!=(secondary?&actual_->secondaryFilter:&actual_->mainFilter)||
           owned->type2Resource!=(secondary?actual_->secondary.resource:actual_->main.resource)||
           owned->identity!=owned->canonicalFilter->identity||
           s->objects[owned->reservedRef-1u].identity!=owned->identity||
           !sameSharedFilterView(s->objects[owned->reservedRef-1u].filter,*owned->coldFilter))return false;
        member=selected.owner;ref=selected.ref;row=owned.get();return true;
    }
    //Clean actual scene190 or positively bound native physical19C route of
    //11FF60/11FE50, followed by the existing
    //typed11F680 and attached120230 adapters. No PostWorld phase or general
    //scene/physics callback is synthesized; dirty scene paths stop explicitly.
    void prepareNativeQueryPrefix(const original_camera_session50KX::Session& expected,
        std::size_t slot,EndCandidateResult& out,bool secondary)noexcept {
        auto& frame=actual_->frame;auto& query=secondary?actual_->secondary:actual_->main;auto& storage=*storage_.get();
        const auto& racer=expected.racerEntities_[slot];
        out.stoppedPC=0x11fe50;
        if(triggerQueryActive_||!query.queryScene.value||
           !g::validVehicleSceneCount49W(frame,gridArena_->bindings))return;
        if(secondary){
            // Actual native coupled-body custody, not an unpublished35C/19C
            // guest Entity. Its registered root belongs to this SAME rig/Bank.
            if(!frame.bodies.main.domain.secondBodyPresent||!gridArena_->rigs[slot]||
               gridArena_->rigs[slot].get()!=gridRigIdentity_[slot]||
               gridArena_->bindings.lifecycle.physical19C!=gridArena_->rigs[slot]->physical||
               !registeredRange(&frame.bodies.secondary,sizeof(frame.bodies.secondary)))return;
        }else if(!racer.body340||!racer.scene190||
           racer.body340->scene288()!=racer.scene190||!racer.scene190->constructed||racer.scene190->released||
           !racer.scene190->entity().reference00||
           racer.scene190->entity().sessionGeneration!=racer.racer.sessionGeneration||
           racer.scene190->sceneIndex!=gridArena_->bindings.lifecycle.main190||
           racer.body340->canonicalNumericAddress()!=&frame.bodies.main)return;
        const auto index=static_cast<int>(secondary?gridArena_->bindings.lifecycle.physical19C:
            gridArena_->bindings.lifecycle.main190);
        if(index<0||static_cast<std::size_t>(index)>=frame.sceneNodeCount)return;
        const auto& node=frame.nodes[index];
        if(!registeredRange(&node,sizeof(node)))return;
        out.stoppedPC=0x28b550;
        //28B59C..5A8 clean bit0 selects only the64-byte world100 copy. The
        //actual scene owner indexes this SAME canonical graph; no shadow pose.
        if(!(node.sceneFlags190&1u)||
           std::memcmp(&node.world100,secondary?&frame.bodies.secondary.root.scene:
               &frame.bodies.main.components.root.scene,sizeof(node.world100))||
           (secondary&&(frame.bodies.secondary.sceneFlags190&255u)!=node.sceneFlags190))return;
        out.stoppedPC=0x11f680;
        g::LiveZoneMembership50EN* selectedMember=nullptr;std::uint32_t ref=0;
        storage::Storage::SharedRosterObject* selectedRow=nullptr;
        if(!selectNativeDetectorMembership(slot,secondary,selectedMember,ref,selectedRow))return;
        SharedOwnerLease sharedLease;
        if(selectedRow&&(selectedRow->effectRevision==std::numeric_limits<std::uint64_t>::max()||
            !beginSharedOwnerLease(sharedLease,out)))return;
        const auto* resource=query.resource;
        if(!resource||resource!=(secondary?gridCompletion_[slot].secondaryResource:gridCompletion_[slot].mainResource)||!resource->nodes||
           !resource->nodeCount||(resource->root&0x8000u)||resource->root>=resource->nodeCount)return;
        const auto& root=resource->nodes[resource->root];
        g::QuerySpherePrepareInput48O input{};input.preparedScene=node.world100;
        input.shapeType=2;input.scenePrepared=true;input.hasPairTree=true;
        for(unsigned i=0;i<3;++i){input.treeCenter.v[i]=root.center[i];input.treeHalf.v[i]=root.half[i];}
        g::QuerySpherePrepareOutput48O prepared{};
        if(g::prepareQuerySphere48O(input,prepared)!=g::QuerySpherePrepareStatus48O::Ready)return;
        if(query.nativeGatePolicy){
            //Same existing native Gate history adaptation: retain the actual
            //previous cache before committing the new source preparation.
            query.nativeGatePreviousWorld=query.worldMatrix;query.nativeGateHasPrevious=true;
        }
        std::memcpy(&query.worldMatrix,&prepared.world,sizeof(query.worldMatrix));
        for(unsigned i=0;i<3;++i)query.sphere.center.v[i]=prepared.center.v[i];
        query.sphere.radius=prepared.radius;
        if(selectedRow){
            auto& mirror=storage.source[selectedRow->reservedRef-1u];
            //Actual source cache preparation is visible BEFORE120230; keep
            //query-written epoch/F8/member fields, not a fresh-state reset.
            mirror.cachedCenter=prepared.center;mirror.cachedRadius=prepared.radius;
            ++selectedRow->effectRevision;sharedLease.wrote_=true;
        }
        (secondary?out.nativeSecondaryCachePrepared:out.nativeQueryCachePrepared)=true;out.stoppedPC=0x120230;
        //The native typed query stores XYZ/radius; prepared.W=1 is retained in
        //the actual bounds below. No full guest cache object is manufactured.
        auto& membership=*selectedMember;
        if(!membership.attached||membership.object!=ref||
           membership.lastShapeTag!=2||membership.lastType2Resource!=resource)return;
        g::LiveZoneBounds50EN bounds{};bounds.bounds.shapeTag=2;
        bounds.bounds.shapeWorld=prepared.world;bounds.bounds.cachedCenter=prepared.center;
        bounds.bounds.cachedRadius=prepared.radius;bounds.type2Resource=resource;
        const auto before=membership.revision;
        const auto update=g::updateLiveZoneMembership50EN(storage.lifecycle,storage.world,
            membership,bounds,storage.membershipWork);
        //Cache writes precede120230 and stay committed if this later bounded
        //membership adapter rejects. No all-prefix rollback or reset replay.
        if(update.status!=g::LiveZoneStatus50EN::Complete)return;
        (secondary?out.nativeSecondaryMembershipCommitted:out.nativeQueryMembershipCommitted)=true;
        (secondary?out.nativeSecondaryMembershipRevision:out.nativeQueryMembershipRevision)=membership.revision;
        if(!membership.attached||membership.object!=ref||membership.lastShapeTag!=2||
           membership.lastType2Resource!=resource||before==std::numeric_limits<std::uint64_t>::max()||
           membership.revision!=before+1u||
           std::memcmp(&membership.lastShapeWorld,&prepared.world,sizeof(prepared.world))||
           std::memcmp(&membership.bounds.cachedCenter,&prepared.center,sizeof(prepared.center))||
           std::memcmp(&membership.bounds.cachedRadius,&prepared.radius,sizeof(prepared.radius)))return;
        out.stoppedPC=0x11ff7c;
        //Publish only the existing native RA prepared-pose mirrors, exactly as
        //the ordinary committed live prefix does. No trigger action/service or
        //geometry callback runs; a missing real observer remains a boundary.
        Diagnostic diagnostic{};
        if(!triggerBankCurrent(expected.actual_.bank,slot,true)||!observeTriggerPoses(diagnostic))return;
        (secondary?out.nativeSecondaryQueryPrefixReturned:out.nativeQueryPrefixReturned)=true;out.stoppedPC=0x2959f4;
        if(selectedRow){sharedLease.completed_=true;endSharedOwnerLease(sharedLease);}
        //The actual limited caller takes its own lexical owner borrow; never
        //nest it inside the prefix's completed membership/cache lease.
        if(!endCandidateProducer_||!endCandidateProducer_->limited||
           !endCandidateProducer_->inCall||!endCandidateProducer_->activeRevision||
           out.projectionRevision!=endCandidateProducer_->activeRevision)return;
        // Explicit native supplied-list policy, not invented guest70/74 fields.
        // Generic typed lease machinery is reused; no MB instance/phase is borrowed.
        const g::SourceQueryIdentity50MB identity{endCandidateProducer_,expected.generation_,
            out.projectionRevision,slot,secondary?actual_->secondaryFilter.identity:actual_->mainFilter.identity};
        endCandidateProducer_->limited(secondary,identity,
            original_follow_query_bridge::QueryBridge::sentinel3E02C0,out);
    }
    //Bounded explicit native295630 policy. Invoked only inside the same
    //candidate lease after actual merge (secondary after main returns); never a later restore or
    //a generic PostWorld callback. Enabled query enters its exact bounded prefix.
    void advanceNativeDetectorPrefix(const original_camera_session50KX::Session& expected,
        std::size_t slot,EndCandidateResult& out,bool secondary)noexcept {
        using Policy=::native_spring_detector_policy50LA::Owner;
        auto* s=storage_.get();auto* policy=s?s->nativeDetectorPolicy_.get():nullptr;
        if(!out.merged||!endCandidateProducer_||!endCandidateProducer_->inCall||
           !endCandidateProducer_->activeRevision||out.projectionRevision!=endCandidateProducer_->activeRevision||
           !policy||!policy->live_||!policy->registered_||
           policy->frame_!=&actual_->frame||policy->bindings_!=&gridArena_->bindings)return;
        auto& e=policy->entries_[secondary?1u:0u];auto& lanes=policy->lanes_[secondary?1u:0u];
        auto& query=secondary?actual_->secondary:actual_->main;
        auto& filter=secondary?actual_->secondaryFilter:actual_->mainFilter;
        g::LiveZoneMembership50EN* membership=nullptr;std::uint32_t ref=0;
        storage::Storage::SharedRosterObject* selectedRow=nullptr;
        if(!selectNativeDetectorMembership(slot,secondary,membership,ref,selectedRow))return;
        if(e.query!=&query||!e.spring||!e.settings||!membership||!ref||!e.query->resource||e.query->filter!=&filter||
           e.query->resource!=(secondary?gridCompletion_[slot].secondaryResource:gridCompletion_[slot].mainResource)||
           !registeredRange(policy->lanes_.data(),sizeof(policy->lanes_))||
           !registeredRange(e.query,sizeof(*e.query))||!registeredRange(e.spring,sizeof(*e.spring))||
           !registeredRange(e.settings,sizeof(*e.settings)))return;
        struct SharedDetectorStop {
            OwnershipBridge& owner;EndCandidateResult& result;bool secondary,shared;
            ~SharedDetectorStop(){if(shared&&!(secondary?result.nativeSecondaryDetectorReturned:result.nativeDetectorReturned))
                owner.sharedEffectsStopped_=true;}
        } stopped{*this,out,secondary,selectedRow!=nullptr};
        auto& app=*expected.actual_.applicationEntities;
        (secondary?out.nativeSecondaryDetectorEntered:out.nativeDetectorEntered)=true;out.stoppedPC=0x295650;
        if(!app.observeNativeDetectorSentinel50LA(0))return;
        out.stoppedPC=0x295670;
        if(lanes.pairs!=Policy::PairPolicy::nativeType2NoPairs)return;
        //No-pair short circuit: actual native point48 is read BEFORE provider
        //policy. Unknown/unrepresented enabled providers never become null.
        if(e.settings->pointContacts48&&lanes.provider!=Policy::ProviderPolicy::nativeAbsent){
            out.stoppedPC=0x29568c;return;
        }
        out.stoppedPC=0x295944;
        if(!app.observeNativeDetectorSentinel50LA(1))return;
        //Actual current registered query lane, not the old admission literal.
        //296490 is the sole mutation here; no scene/cache/query/phase replay.
        if(e.query->resetRecordsBefore){
            g::resetSpringRecords48N(e.spring->records);(secondary?out.nativeSecondaryDetectorReset:out.nativeDetectorReset)=true;
        }
        if(!lanes.nativeEnabled){
            (secondary?out.nativeSecondaryDetectorReturned:out.nativeDetectorReturned)=true;
            out.stoppedPC=secondary?0x219c10:0x219be4;return;
        }
        out.stoppedPC=0x2959ec;
        prepareNativeQueryPrefix(expected,slot,out,secondary); //Same actual candidate lease, after reset only.
    }
    bool readNativeContactPodCategory(const g::OriginalApplicationEntities50LA& app,
        std::uint32_t& value)const noexcept {
        if(app.nativeOwnerThread_!=std::this_thread::get_id())return false;
        for(const auto& binding:app.nativeCollisionCategoryBindings_)
            if(std::string_view(binding.name)=="POD"){
                if(!binding.bit||!actual_||!actual_->recovery||!actual_->recovery->sourcePrepared||
                   actual_->recovery->globals.extraPodCategory3B14B0!=binding.bit)return false;
                value=binding.bit;return true;
            }
        return false;
    }
    void advanceNativeContactPrefix(const original_camera_session50KX::Session& expected,
        std::size_t slot,EndCandidateResult& out)noexcept {
        out.stoppedPC=0x219440;
        if(!endCandidateProducer_||!endCandidateProducer_->inCall||!out.firstReturned||
           out.projectionRevision!=endCandidateProducer_->activeRevision||
           !(out.nativeSecondarySkipped||out.nativeSecondaryDetectorReturned))return;
        struct SharedContactStop {
            OwnershipBridge& owner;EndCandidateResult& result;bool shared;
            ~SharedContactStop(){if(shared&&!result.nativeContactReturned)owner.sharedEffectsStopped_=true;}
        } stopped{*this,out,bool(storage_.get()->sharedRoster_)};
        auto& app=*expected.actual_.applicationEntities;
        if(!app.enterNativeVehicleContacts219420())return;
        out.nativeContactStaticsEntered=true;out.stoppedPC=0x2194d0;
        const auto& racer=expected.racerEntities_[slot];
        const auto* owner=racer.chariotModel1B4;
        const auto* model=owner?owner->model():nullptr;
        if(!owner||!owner->constructed2403A0()||owner->released()||!model||
           !model->constructed||model->released||!model->entity().reference00||
           model->entity().sessionGeneration!=racer.racer.sessionGeneration||
           !gridArena_->rigs[slot]||gridArena_->rigs[slot].get()!=gridRigIdentity_[slot]||
           model->sceneIndex!=gridArena_->bindings.lifecycle.model1B4||
           model->sceneIndex!=gridArena_->rigs[slot]->roots[native_racer50HW::Rig::chariot])return;
        auto& frame=actual_->frame;const auto index=static_cast<int>(model->sceneIndex);
        if(!g::validVehicleSceneCount49W(frame,gridArena_->bindings)||index<0||
           static_cast<std::size_t>(index)>=frame.sceneNodeCount)return;
        auto& node=frame.nodes[index];if(!registeredRange(&node,sizeof(node)))return;
        out.stoppedPC=0x28b1e0;
        //Exact parentless getter leaf, not generic scene update/observer work.
        //A clean cache ignores parent, as the actual28B1A0/28A700 do.
        if(!(node.sceneFlags190&1u)){
            out.stoppedPC=0x28b1fc;
            if(node.parent180!=g::SceneNodeIndex49P::none)return;
            std::memcpy(&node.world100,&node.localC0,sizeof(node.world100));
            node.sceneFlags190=static_cast<std::uint8_t>((node.sceneFlags190&0xfeu)|1u);
        }
        //Getters write XYZ only. Their zeroed caller scratch W is not replaced
        //by a synthetic1 and no normalization/finiteness rewrite is performed.
        std::memcpy(out.nativeContactPosition.data(),node.world100.v+12,3*sizeof(float));
        out.stoppedPC=0x28a760;
        std::memcpy(out.nativeContactForward.data(),node.world100.v+8,3*sizeof(float));
        std::memcpy(out.nativeContactUp.data(),node.world100.v+4,3*sizeof(float));
        out.nativeContactScenePrepared=true;out.stoppedPC=0x2197d0;
        if(endCandidateProducer_->contact)endCandidateProducer_->contact(app,out);
    }
    // Only the actual Setup Context may lend its concrete materializer. This
    // is NOT collectRegistered/a geometry callback or a public readiness API.
    // The borrow expires before Context's captures, or is revoked at close.
    struct EndEngineEnableReader {
        const void* context=nullptr;
        bool (*read)(const void*,bool&)noexcept=nullptr;
        const original_camera_session50KX::Session* session=nullptr;
        std::optional<std::uint32_t> sourceFactorF20Bits; //Actual Race scheduling capture, not native schedule.
    };
    struct EndCandidateProducer {
        std::weak_ptr<OwnershipBridge> owner;
        storage::Storage* storage=nullptr;
        native_dynamic_contact50IW::Registry* bodyRegistry=nullptr;
        const original_camera_session50KX::Session* activeSession=nullptr;
        const EndEngineEnableReader* engineEnable=nullptr; //Lexical Race-owned reader only.
        std::size_t activeSlot=0;
        std::function<void(EndCandidateResult&)> run,sharedRun;
        std::function<void(const g::OriginalApplicationEntities50LA&,EndCandidateResult&)> contact;
        std::function<void(SharedTerrainInvocation&)> terrainLimited;
        std::function<void(SharedTerrainInvocation&)> springResponse;
        g::CollisionResultBindingOwner50MB nativeLimitedResultBorrow;
        std::function<void(bool,const g::SourceQueryIdentity50MB&,const g::SpringVector4_48N&,
            EndCandidateResult&)> limited;
        //Private actual direct POD candidate service, called only from shared
        //typed source selection. Secondary reached-body callbacks still stop;
        //the fresh shared factory remains uncalled until all consumers join.
        std::function<bool(bool,std::size_t,const g::SourceQueryIdentity50MB&,
            const g::SourceOuterResultLease50MB&,EndCandidateResult&)> directPod;
        std::array<std::uint64_t,2> directPodSerial{};std::array<std::size_t,2> directPodOrdinal{};
        std::uint64_t serial=0,activeRevision=0;
        bool inCall=false;
        ~EndCandidateProducer(){if(auto o=owner.lock();o&&o->endCandidateProducer_==this)
            o->endCandidateProducer_=nullptr;}
        bool projected()noexcept {
            if(serial==std::numeric_limits<std::uint64_t>::max())return false;
            ++serial;activeRevision=inCall?serial:0;return true;
        }
    };
    EndCandidateProducer* endCandidateProducer_=nullptr;
    bool runEndFirstCandidateQueries(const original_camera_session50KX::Session& expected,
        std::size_t slot,EndCandidateResult& output,const EndEngineEnableReader& engineEnable)noexcept {
        //A prior source-returned first query cannot be reprojected/replayed
        //after incomplete publication. Refuse BEFORE any Bank/producer work.
        if(sharedEffectsStopped_||sharedOwnerLease_||sharedRoleViews_)return false;
        if(!replayRacerIdentityCurrent(expected,slot)||!gridInputsRegistered(*gridArena_)||
           !endCandidateProducer_||endCandidateProducer_->storage!=storage_.get()||
           endCandidateProducer_->inCall||endCandidateProducer_->engineEnable||!endCandidateProducer_->run)return false;
        auto& producer=*endCandidateProducer_;
        if(producer.owner.lock().get()!=this)return false;
        //Native identity registration is representation work, before ANY
        //source projection/query effect or Bank visit. Subsequent invocations
        //verify the same retained owners/tokens; never reallocate identities.
        //Failure has no token side effects and is not a new source gate:
        //static/skip paths do not need secondary callbacks. If one is reached,
        //its typed owner check below requires actual successful admission.
        if(storage_.get()->sharedRoster_&&producer.bodyRegistry&&producer.bodyRegistry->prepared()){
            (void)registerNativeSecondaryContactRegistry(*producer.bodyRegistry);
            (void)registerNativeRacerTerrainRegistry(expected,*producer.bodyRegistry);
        }
        bool entered=false;
        try {expected.actual_.bank.visit(slot,[&](){
            auto& arena=*gridArena_;const auto& racer=expected.racerEntities_[slot];
            if(!arena.rigs[slot]||arena.rigs[slot].get()!=gridRigIdentity_[slot]||
               actual_->main.resource!=gridCompletion_[slot].mainResource||
               !racer.body340||racer.body340->canonicalNumericAddress()!=&actual_->frame.bodies.main)return;
            producer.inCall=true;producer.activeRevision=0;
            producer.activeSession=&expected;producer.activeSlot=slot;producer.engineEnable=&engineEnable;
            struct Expire {OwnershipBridge& owner;EndCandidateProducer& p;EndCandidateResult& out;
                ~Expire(){owner.finishSharedCollectionInvocation(out);p.engineEnable=nullptr;p.activeSession=nullptr;p.activeRevision=0;p.inCall=false;}
            } expire{*this,producer,output};
            entered=true;output={};
            if(storage_.get()->sharedRoster_){if(!producer.sharedRun)return;producer.sharedRun(output);}
            else producer.run(output);
            if(output.merged)advanceNativeDetectorPrefix(expected,slot,output,false);
            if(output.nativeDetectorReturned){
                output.stoppedPC=0x219be4;
                auto& frame=actual_->frame;
                //Positive native35C adaptation: a real coupled secondary root
                //and current rig are required. False/unpublished is not null.
                if(!frame.bodies.main.domain.secondBodyPresent||
                   arena.bindings.lifecycle.physical19C!=arena.rigs[slot]->physical||
                   actual_->secondary.resource!=gridCompletion_[slot].secondaryResource||
                   !registeredRange(&frame.bodies.secondary,sizeof(frame.bodies.secondary))||
                   !registeredRange(&frame.bodies.vehicle838,sizeof(frame.bodies.vehicle838)))return;
                output.nativeSecondaryBodyBound=true;output.stoppedPC=0x219bf0;
                if(!frame.bodies.vehicle838){
                    output.nativeSecondarySkipped=true;output.stoppedPC=0x219c10;
                }else {
                    output.stoppedPC=0x219bfc;
                    advanceNativeDetectorPrefix(expected,slot,output,true);
                }
            }
            if(output.nativeSecondarySkipped||output.nativeSecondaryDetectorReturned)
                advanceNativeContactPrefix(expected,slot,output);
            if(output.nativeContactReturned)advanceNativeTerrainPreparePrefix(expected,slot,output);
            // No compact table publication survives this synchronous lease.
            // A later rejected query is an explicit stopped invocation, not
            // authority to refresh, restore another slot and replay query1.
        });}catch(...){return entered;}
        return entered;
    }
    bool replayRacerIdentityCurrent(const original_camera_session50KX::Session& expected,
        std::size_t slot)const noexcept {
        if(terminal_||closed_||cameraPrepareTerminal_||!cameraBankCurrent()||
           !camera_||!camera_->session||camera_->session.get()!=&expected||
           !actual_||!gridArena_||!freshLighting_||camera_->lighting!=freshLighting_||
           slot>=inputs_.collisionRoster.size()||(rosterResources_&&!rosterSlotComplete_[slot])||
           &actual_->frame!=&expected.actual_.arena||&expected.actual_.bank!=bankIdentity_||
           expected.closed_||expected.abandoned_||!expected.coldBound_||!expected.readsBound_||
           expected.generation_!=camera_->generation||!expected.actual_.applicationEntities)return false;
        const auto& entity=expected.racerEntities_[slot];
        if(!entity.prepared()||entity.racer.objectKey!=expected.racerKey(slot)||
           entity.racer.sessionGeneration!=expected.generation_||
           entity.actualCounter423F60()!=&expected.actual_.applicationEntities->objectCount423F60()||
           entity.actualLock423F58()!=&expected.actual_.applicationEntities->attachmentLock423F58())return false;
        try {return expected.identitiesCurrent();}catch(...){return false;}
    }
    bool readReplayMode3C229F94(const original_camera_session50KX::Session& expected,
        std::size_t slot,std::uint32_t& value)const noexcept {
        return replayRacerIdentityCurrent(expected,slot)&&
            expected.racerEntities_[slot].readReplayMode3C229F94(value);
    }
    bool readEndPhysicsState40(const original_camera_session50KX::Session& expected,
        std::size_t slot,std::uint32_t& value)const noexcept {
        if(!replayRacerIdentityCurrent(expected,slot))return false;
        try {value=expected.actual_.bank.read(slot,expected.reads_.state40);return true;}
        catch(...){return false;}
    }
    bool readEndProbe10421990C(const original_camera_session50KX::Session& expected,
        std::size_t slot,std::uint32_t& value)const noexcept {
        if(!replayRacerIdentityCurrent(expected,slot)||!gridInputsRegistered(*gridArena_))return false;
        try {
            const auto lane=expected.actual_.bank.bindRead(actual_->frame.bodies.main.components.probe348.field104);
            value=expected.actual_.bank.read(slot,lane);return true;
        }catch(...){return false;}
    }
    bool readEndGroundedBrake219924(const original_camera_session50KX::Session& expected,
        std::size_t slot,std::uint32_t& bits)const noexcept {
        if(!replayRacerIdentityCurrent(expected,slot)||!gridInputsRegistered(*gridArena_)||
           slot>=stagedAero_.size()||slot>=stagedPods694_.size()||
           !expected.actual_.actualChariotModels||
           slot>=expected.actual_.actualChariotModels->size())return false;
        const auto retained=stagedAero_[slot].lock();const auto& racer=expected.racerEntities_[slot];
        if(!retained||!retained->published_||retained->revoked_||
           retained->bank_!=&expected.actual_.bank||retained->frame_!=&actual_->frame||
           retained->parameters_!=&gridArena_->parameters.bodies.main||
           retained->application_!=expected.actual_.applicationEntities.get()||
           retained->application_->nativeOwnerThread_!=std::this_thread::get_id()||
           retained->slot_!=slot||retained->generation_!=expected.generation_||
           retained->model_!=(*expected.actual_.actualChariotModels)[slot].get()||
           retained->racer_!=&racer||!retained->racerKey_||
           racer.racer.objectKey!=retained->racerKey_||
           !racer.body340||retained->body_!=racer.body340||
           racer.body340!=stagedPods694_[slot].bodyA0||
           racer.body340->canonicalNumericAddress()!=&actual_->frame.bodies.main||
           racer.body340->scene288()!=racer.scene190)return false;
        try {
            //Same existing native Aero350+EC adaptation as advanceSharedAero.
            //Read the actual selected Bank lane, not another live slot, copied
            //PostWorld input, a supplied brake, or a fabricated guest350 Entity.
            const auto lane=expected.actual_.bank.bindRead(actual_->frame.bodies.main.components.bindings.aeroBrakeEC);
            bits=std::bit_cast<std::uint32_t>(expected.actual_.bank.read(slot,lane));return true;
        }catch(...){return false;}
    }
    bool readEndGroundedSpeed219948(const original_camera_session50KX::Session& expected,
        std::size_t slot,std::uint32_t& bits)const noexcept {
        if(!replayRacerIdentityCurrent(expected,slot)||!gridInputsRegistered(*gridArena_)||
           slot>=stagedPods694_.size())return false;
        const auto& racer=expected.racerEntities_[slot];
        if(!racer.body340||racer.body340!=stagedPods694_[slot].bodyA0||
           racer.body340->canonicalNumericAddress()!=&actual_->frame.bodies.main||
           !racer.body340->entity().reference00||
           racer.body340->entity().parent1C!=&racer.racer||
           racer.body340->entity().sessionGeneration!=expected.generation_)return false;
        try {
            //Exact cached340+34 reload. Never recompute speed from velocity.
            const auto lane=expected.actual_.bank.bindRead(actual_->frame.bodies.main.cachedSpeed34);
            bits=std::bit_cast<std::uint32_t>(expected.actual_.bank.read(slot,lane));return true;
        }catch(...){return false;}
    }
    bool readEndGroundedAxes2199B8(const original_camera_session50KX::Session& expected,
        std::size_t slot,std::array<std::uint32_t,4>& forward,
        std::array<std::uint32_t,4>& up,std::uint32_t& pc)noexcept {
        pc=0x2199b4;
        if(!replayRacerIdentityCurrent(expected,slot)||!gridInputsRegistered(*gridArena_))return false;
        bool copied=false;
        try {expected.actual_.bank.visit(slot,[&](){
            const auto& racer=expected.racerEntities_[slot];const auto* scene=racer.scene190;
            auto& frame=actual_->frame;
            if(!scene||!scene->constructed||scene->released||!scene->entity().reference00||
               scene->entity().sessionGeneration!=expected.generation_||
               !gridArena_->rigs[slot]||gridArena_->rigs[slot].get()!=gridRigIdentity_[slot]||
               scene->sceneIndex!=gridArena_->rigs[slot]->main||
               scene->sceneIndex!=gridArena_->bindings.lifecycle.main190||
               !g::validVehicleSceneCount49W(frame,gridArena_->bindings))return;
            const auto index=static_cast<int>(scene->sceneIndex);
            if(index<0||static_cast<std::size_t>(index)>=frame.sceneNodeCount)return;
            auto& node=frame.nodes[index];pc=0x28a760;
            if(!registeredRange(&node,sizeof(node)))return;
            if(!(node.sceneFlags190&1u)){
                pc=0x28a770;
                if(node.parent180!=g::SceneNodeIndex49P::none){
                    pc=0x28a77c;return; //Before ancestor2897B0/product289940.
                }
                pc=0x28a7a4;
                //Same logical main190 cache and native body mirrors as the
                //actual retained340 owner. Never replace another slot's scene.
                auto& numericScene=frame.bodies.main.components.root.scene;
                auto& numericFlags=frame.bodies.mainSphere.sceneFlags190;
                if(slot>=stagedPods694_.size()||!racer.body340||
                   racer.body340!=stagedPods694_[slot].bodyA0||
                   racer.body340->canonicalNumericAddress()!=&frame.bodies.main||
                   racer.body340->scene288()!=scene||
                   !racer.body340->entity().reference00||
                   racer.body340->entity().parent1C!=&racer.racer||
                   racer.body340->entity().sessionGeneration!=expected.generation_||
                   !registeredRange(&numericScene,sizeof(numericScene))||
                   !registeredRange(&numericFlags,sizeof(numericFlags)))return;
                //Eight source two-word iterations, not a staged whole matrix.
                //Both loads precede either store. No finiteness admission:
                //NaN/signzero payloads are copied exactly, including dead W.
                for(unsigned pair=0;pair<8u;++pair){
                    const unsigned lane=2u*pair;std::uint32_t firstWord,secondWord;
                    pc=0x28a7ac;std::memcpy(&firstWord,node.localC0.v+lane,4);
                    pc=0x28a7b0;std::memcpy(&secondWord,node.localC0.v+lane+1u,4);
                    pc=0x28a7b8;
                    std::memcpy(node.world100.v+lane,&firstWord,4);
                    std::memcpy(numericScene.v+lane,&firstWord,4);
                    pc=0x28a7bc;
                    std::memcpy(node.world100.v+lane+1u,&secondWord,4);
                    std::memcpy(numericScene.v+lane+1u,&secondWord,4);
                }
                pc=0x28a7cc; //Reload flags AFTER all sixteen world writes.
                const auto flags=static_cast<std::uint8_t>((node.sceneFlags190&0xfeu)|1u);
                pc=0x28a7e0;node.sceneFlags190=flags;
                numericFlags=(numericFlags&0xffffff00u)|flags;
                //Native mirrors are the same logical stores. Bank::visit saves
                //the selected slot even on later rejection; caller already set
                //its terminal prefix cut before entering this getter. No retry.
            }
            if(std::memcmp(&node.world100,&frame.bodies.main.components.root.scene,
                           sizeof(node.world100)))return;
            //Positive Scene190 and literal relative=null. Clean branch ignores
            //parent; source28A7EC..820 copies XYZ payloads without arithmetic.
            //No VF30 or inherited VU lane is read on this admitted leaf.
            for(unsigned i=0;i<3;++i){
                pc=0x28a7f0+8*i;std::memcpy(&forward[i],node.world100.v+8+i,4);
            }
            for(unsigned i=0;i<3;++i){
                pc=0x28a810+8*i;std::memcpy(&up[i],node.world100.v+4+i,4);
            }
            pc=0x2199c0;copied=true;
        });}catch(...){return false;}
        return copied;
    }
    bool readEndGrounded3782199E4(const original_camera_session50KX::Session& expected,
        std::size_t slot,std::uint32_t& bits)const noexcept {
        if(!replayRacerIdentityCurrent(expected,slot)||!gridInputsRegistered(*gridArena_))return false;
        try {
            const auto lane=expected.actual_.bank.bindRead(gridArena_->selectors.vehicle378);
            bits=std::bit_cast<std::uint32_t>(expected.actual_.bank.read(slot,lane));return true;
        }catch(...){return false;}
    }
    bool readEndAirborne37C219A74(const original_camera_session50KX::Session& expected,
        std::size_t slot,std::uint32_t& bits)const noexcept {
        if(!replayRacerIdentityCurrent(expected,slot)||!gridInputsRegistered(*gridArena_))return false;
        try {
            //The real Fixture selector is the existing authored/current37C
            //provider consumed by49W. Bind its actual registered Bank lane;
            //do not read another slot's live arena or create default gravity.
            const auto lane=expected.actual_.bank.bindRead(gridArena_->selectors.vehicle37C);
            bits=std::bit_cast<std::uint32_t>(expected.actual_.bank.read(slot,lane));return true;
        }catch(...){return false;}
    }
    bool storeEndAirborneGravity219A88(const original_camera_session50KX::Session& expected,
        std::size_t slot,std::uint32_t f12Bits)noexcept {
        if(!replayRacerIdentityCurrent(expected,slot)||!gridInputsRegistered(*gridArena_))return false;
        bool stored=false;
        try {
            expected.actual_.bank.visit(slot,[&](){
                auto& arena=*gridArena_;auto& frame=actual_->frame;
                const auto& racer=expected.racerEntities_[slot];
                if(!arena.rigs[slot]||arena.rigs[slot].get()!=gridRigIdentity_[slot]||
                   arena.bindings.lifecycle.main190!=arena.rigs[slot]->main||
                   arena.bindings.lifecycle.physical19C!=arena.rigs[slot]->physical||
                   actual_->main.resource!=gridCompletion_[slot].mainResource||
                   !racer.body340||!racer.gravity344||
                   racer.body340->canonicalNumericAddress()!=&frame.bodies.main||
                   racer.gravity344->canonicalAccelerationAddress()!=&frame.bodies.main.selectedGravity||
                   racer.gravity344->target50()!=racer.body340)return;
                stored=racer.gravity344->storeEndAcceleration1143E0(
                    *expected.actual_.applicationEntities,racer.racer,*racer.body340,
                    frame.bodies.main,f12Bits);
            });
        }catch(...){return false;}
        return stored;
    }
    bool readEndNativeBoost64(const original_camera_session50KX::Session& expected,
        std::size_t slot,std::uint32_t& value)const noexcept {
        if(!replayRacerIdentityCurrent(expected,slot)||!gridInputsRegistered(*gridArena_)||
           !registeredRange(&expected.actual_.boost,sizeof(expected.actual_.boost)))return false;
        try {
            //Existing actual Session borrow is BoostState, not HandlingTuning's
            //distinct BoostParameters. Its whole state must be Bank-registered.
            const auto lane=expected.actual_.bank.bindRead(expected.actual_.boost.vehicle64);
            value=expected.actual_.bank.read(slot,lane);return true;
        }catch(...){return false;}
    }
    bool readEndNativeFlagsB9C(const original_camera_session50KX::Session& expected,
        std::size_t slot,std::uint8_t& value)const noexcept {
        if(!replayRacerIdentityCurrent(expected,slot)||!gridInputsRegistered(*gridArena_))return false;
        try {
            const auto lane=expected.actual_.bank.bindRead(actual_->frame.bodies.main.controls.player.flagsB9C);
            value=expected.actual_.bank.read(slot,lane);return true;
        }catch(...){return false;}
    }
    enum class EndNativeEngineLane:std::uint8_t {engine34,engine38};
    bool storeEndNativeEngineLane(const original_camera_session50KX::Session& expected,
        std::size_t slot,EndNativeEngineLane lane,std::uint32_t sourceBoolean)noexcept {
        if(sourceBoolean>1u||!replayRacerIdentityCurrent(expected,slot)||
           !gridInputsRegistered(*gridArena_))return false;
        bool stored=false;
        try {
            expected.actual_.bank.visit(slot,[&](){
                auto& arena=*gridArena_;auto& frame=actual_->frame;
                const auto& racer=expected.racerEntities_[slot];
                if(!arena.rigs[slot]||arena.rigs[slot].get()!=gridRigIdentity_[slot]||
                   arena.bindings.lifecycle.main190!=arena.rigs[slot]->main||
                   arena.bindings.lifecycle.physical19C!=arena.rigs[slot]->physical||
                   actual_->main.resource!=gridCompletion_[slot].mainResource||
                   !racer.body340||racer.body340->canonicalNumericAddress()!=&frame.bodies.main||
                   racer.body340->entity().parent1C!=&racer.racer||
                   racer.body340->entity().reference00!=1||
                   racer.body340->entity().sessionGeneration!=racer.racer.sessionGeneration)return;
                //EXPLICIT EXISTING NATIVE ENGINE-FIELD ADAPTATION.49W/49K
                //consumers already map these sole lanes to EngineInput48D
                //boost(+34)/repair(+38). No guest34C Entity/publication or
                //virtual side effect is asserted by these exact scalar stores.
                if(lane==EndNativeEngineLane::engine34)
                    frame.bodies.main.selectedEngineFlags.engine34=sourceBoolean; //219AB8.
                else frame.bodies.main.selectedEngineFlags.engine38=sourceBoolean; //219AD0.
                stored=true;
            });
        }catch(...){return false;}
        return stored;
    }
    bool readReplayRetirementCD4(const original_camera_session50KX::Session& expected,
        std::size_t slot,std::uint32_t& value)const noexcept {
        if(!replayRacerIdentityCurrent(expected,slot))return false;
        try {value=expected.actual_.bank.read(slot,expected.reads_.retirementCD4);return true;}
        catch(...){return false;}
    }
    bool readReplayRecordedZoneC38(const original_camera_session50KX::Session& expected,
        std::size_t slot,std::uint32_t& value)const noexcept {
        return replayRacerIdentityCurrent(expected,slot)&&
            freshLighting_->readRecordedZoneC38(static_cast<std::uint32_t>(slot),value);
    }
    bool readReplayRecordedCenterC40(const original_camera_session50KX::Session& expected,
        std::size_t slot,std::array<std::uint32_t,4>& value)const noexcept {
        return replayRacerIdentityCurrent(expected,slot)&&
            freshLighting_->readRecordedCenterC40(static_cast<std::uint32_t>(slot),value);
    }
    // Private same-world Bank getter/229F20 execution, no supplied racer
    //pointer or callback. Future complete end admission remains outside this.
    bool recordEndRacer229F20(const original_camera_session50KX::Session& expected,
        std::size_t slot,std::uint32_t& pc)noexcept {
        pc=0x1a8094;
        if(terminal_||closed_||cameraPrepareTerminal_||!cameraBankCurrent()||
           !camera_||!camera_->session||camera_->session.get()!=&expected||
           !actual_||!gridArena_||!freshLighting_||camera_->lighting!=freshLighting_||
           slot>=inputs_.collisionRoster.size()||(rosterResources_&&!rosterSlotComplete_[slot])||!freshComplete_[slot]||
           &actual_->frame!=&expected.actual_.arena||
           &expected.actual_.bank!=bankIdentity_||expected.closed_||expected.abandoned_||
           !expected.coldBound_||!expected.readsBound_||expected.generation_!=camera_->generation)
            return false;
        // Borrowed actual scene/frame/parameters follow the same Bank visit.
        // No read-only snapshot, fresh-grid proof, or fabricated694 is passed.
        auto& bank=expected.actual_.bank;
        bool returned=false;
        try {
            bank.visit(slot,[&](){
                pc=0x229f20;
                auto& arena=*gridArena_;
                if(!arena.rigs[slot]||arena.rigs[slot].get()!=gridRigIdentity_[slot]||
                   arena.bindings.lifecycle.main190!=arena.rigs[slot]->main||
                   arena.bindings.lifecycle.physical19C!=arena.rigs[slot]->physical||
                   actual_->main.resource!=gridCompletion_[slot].mainResource)return;
                g::original_gc_gameplay_lighting_candidate::Diagnostic detail;
                returned=freshLighting_->recordAttachedEndRacer229F20(static_cast<std::uint32_t>(slot),
                    actual_->frame,arena.bindings,arena.parameters,detail);
                pc=detail.sourcePC;
            });
        }catch(...){return false;} //Earlier canonical C38 survives any rejection.
        return returned;
    }
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::native_racer_model50LA::Owner; //Actual source348 caller only.
    const ::ra_pod_scene50LA::Owner* sceneOwner_=nullptr;
    friend class ::original_main_entry50KG::NativeCollectorFifo50LA;
    friend class ::original_cable_selection50LA::Binding;
    friend class ra_cold_aggregate50LA::Owner;
    friend class ra_cold_roster50LA::Operation;
    friend class ra_cold_roster50LA::FreshExecutionArena;
    friend class original_camera_session50KX::Session;
    std::shared_ptr<const ra_cold_roster50LA::Resources> rosterResources_;
    const terrain_probe_authoring50fq_fixture::Owner* rosterTerrain_=nullptr;
    const g::Type3QueryInput48P* rosterMovement_=nullptr;
    std::array<std::uint32_t,3> rosterAuxiliaryIds_{}; // terrain,694,7A4
    struct BodyIdentityReservation {
        const native_racer_collision50HW::Owner* collision=nullptr;
        std::uint32_t main=0,secondary=0,terrain=0,pod=0,movement=0;
    };
    std::array<BodyIdentityReservation,8> bodyIdentities_{};
    //Private, unconnected source-mode prerequisite. These slots retain only
    //immutable authoring and actual arena addresses; no Bank snapshot or
    //prepared/coldBound/Session completion is substituted for source setup.
    std::array<terrain_probe_authoring50fq_fixture::SourceStage348,8> source348Stages_{};
    std::array<g::OriginalRacerTerrainQueryOwner50LA*,8> source348CountedOwners_{};
    bool source348Arena_=false;
    // Presence only: keep Storage custody inside its existing bridge friend.
    // This does not claim that the allocation is live, bound or prepared.
    bool hasSharedRosterAllocation()const noexcept {
        const auto* s=storage_.get();return s&&bool(s->sharedRoster_);
    }
    bool allocateSource348NamespaceBeforeBank(terrain_probe_authoring50fq_fixture::Owner& owner,
        live_terrain_probe50fr::Owner& live,
        const terrain_probe_authoring50fq_fixture::Bindings& binding,std::string& error){
        if(source348Arena_||bindAttempted_||actual_||bankIdentity_||!bankLifetime_.expired()||
           worldOwnersRegistered_||live.initialized||owner.prepared)return false;
        if(!terrain_probe_authoring50fq_fixture::SourceStage348::allocateNamespace(owner,binding,error))return false;
        source348Arena_=true;return true;
    }
    bool reserveSource348InActualSlot(std::size_t slot,
        const g::VehicleFrameParameters49W& parameters,const std::vector<std::uint8_t>& phy,
        const terrain_probe_authoring50fq_fixture::Bindings& binding,std::string& error){
        auto* s=storage_.get();const auto pin=bankLifetime_.lock();
        if(!source348Arena_||!s||!s->sharedRoster_||!actual_||terminal_||closed_||!pin||
           !bankIdentity_||pin->identity!=bankIdentity_||bankIdentity_->raWorldLifetime_.get()!=pin.get()||
           !bankIdentity_->sealed_||bankIdentity_->retired_||bankIdentity_->visitDepth_!=1u||
           bankIdentity_->currentSlot_!=slot||slot>=inputs_.collisionRoster.size()||slot>=source348Stages_.size()||
           !registeredRange(&parameters,sizeof(parameters))||
           !registeredRange(&actual_->terrain.initial,sizeof(actual_->terrain.initial)))return false;
        const auto& ids=bodyIdentities_[slot];const auto* collision=inputs_.collisionRoster[slot];
        auto& row=s->sharedRoster_->objects[slot*storage::Storage::rosterRoles+
            static_cast<std::size_t>(storage::Storage::RosterRole::terrain348)];
        if(!collision||ids.collision!=collision||binding.identity!=ids.terrain||
           actual_->main.resource!=&collision->main()||!row||row->slot!=slot||
           row->collision!=collision||row->coldBound||!row->terrainResource||row->terrainResource->initialized||
           row->membership.state.attached||row->membership.state.object)return false;
        //No canonical numeric field/query shape is published; body mass and
        //7EC are not read. Future constructor-only Session custody is separate.
        return source348Stages_[slot].reserve(actual_->terrain,actual_->terrainLive,
            actual_->main,*collision,parameters,phy,binding,error);
    }
    terrain_probe_authoring50fq_fixture::SourceStage348* admitSource348CountedConstructor212060(std::size_t slot,
        const g::OriginalRacerMainBodyOwner50LA& body)noexcept {
        auto* s=storage_.get();const auto pin=bankLifetime_.lock();
        if(!source348Arena_||!s||!s->sharedRoster_||!actual_||terminal_||closed_||!pin||
           !bankIdentity_||pin->identity!=bankIdentity_||bankIdentity_->raWorldLifetime_.get()!=pin.get()||
           !bankIdentity_->sealed_||bankIdentity_->retired_||bankIdentity_->visitDepth_!=1u||
           bankIdentity_->currentSlot_!=slot||slot>=source348Stages_.size()||slot>=s->sharedRoster_->count||
           body.canonicalNumericAddress()!=&actual_->frame.bodies.main||body.entity().reference00!=1u||
           !body.entity().sessionGeneration||!body.scene288()||!body.scene288()->constructed||body.scene288()->released)return nullptr;
        auto& row=s->sharedRoster_->objects[slot*storage::Storage::rosterRoles+
            static_cast<std::size_t>(storage::Storage::RosterRole::terrain348)];
        if(!row||row->coldBound||row->nativeTerrainBody128||row->generation||
           row->membership.state.attached||row->membership.state.object)return nullptr;
        //Admission only. The actual counted1B0EE0 owner performs ordered stores.
        return &source348Stages_[slot];
    }
    bool publishSource348Body128(std::size_t slot,g::OriginalRacerTerrainQueryOwner50LA& query,
        const g::OriginalRacerMainBodyOwner50LA& body)noexcept {
        auto* stage=admitSource348CountedConstructor212060(slot,body);
        if(!stage||query.stage_!=stage||query.slot_!=slot||!query.setupComplete_||query.retired_||
           !query.body128_||*query.body128_!=&body||query.base_.entity_.reference00!=1u||
           query.base_.entity_.sessionGeneration!=body.entity().sessionGeneration||source348CountedOwners_[slot])return false;
        auto& row=storage_.get()->sharedRoster_->objects[slot*storage::Storage::rosterRoles+
            static_cast<std::size_t>(storage::Storage::RosterRole::terrain348)];
        row->nativeTerrainBody128=&body;row->generation=body.entity().sessionGeneration;
        source348CountedOwners_[slot]=&query; //Native borrow only; no coldBound/shape/prepared.
        return true;
    }
    bool withdrawSource348Body128(std::size_t slot,g::OriginalRacerTerrainQueryOwner50LA& query)noexcept {
        const auto pin=bankLifetime_.lock();auto* s=storage_.get();
        if(!pin||!bankIdentity_||pin->identity!=bankIdentity_||!s||!s->sharedRoster_||
           slot>=source348CountedOwners_.size()||slot>=s->sharedRoster_->count)return false;
        if(!source348CountedOwners_[slot])return true;
        auto& row=s->sharedRoster_->objects[slot*storage::Storage::rosterRoles+
            static_cast<std::size_t>(storage::Storage::RosterRole::terrain348)];
        if(source348CountedOwners_[slot]!=&query||!row||row->coldBound||
           !query.body128_||row->nativeTerrainBody128!=*query.body128_)return false;
        row->nativeTerrainBody128=nullptr; //Native borrowed projection, not source128 store.
        source348CountedOwners_[slot]=nullptr;return true;
    }
    bool writeSource348ScalarBefore21494C(std::size_t slot,
        const g::OriginalRacerEntityCold50LA& racer,float actualSourceValue)noexcept {
        auto* s=storage_.get();const auto pin=bankLifetime_.lock();
        if(!source348Arena_||!s||!s->sharedRoster_||!actual_||terminal_||closed_||!pin||
           !bankIdentity_||pin->identity!=bankIdentity_||bankIdentity_->raWorldLifetime_.get()!=pin.get()||
           !bankIdentity_->sealed_||bankIdentity_->retired_||bankIdentity_->visitDepth_!=1u||
           bankIdentity_->currentSlot_!=slot||slot>=source348Stages_.size()||slot>=s->sharedRoster_->count||
           !racer.prepared()||!racer.body340||racer.body340->canonicalNumericAddress()!=&actual_->frame.bodies.main)return false;
        const auto& row=s->sharedRoster_->objects[slot*storage::Storage::rosterRoles+
            static_cast<std::size_t>(storage::Storage::RosterRole::terrain348)];
        if(!row||row->coldBound||row->nativeTerrainBody128!=racer.body340||
           row->generation!=racer.racer.sessionGeneration)return false;
        const auto* counted=source348CountedOwners_[slot];
        if(!counted||!counted->appended()||counted->retired_||counted->stage_!=&source348Stages_[slot]||
           !racer.sourceTerrain348_||*racer.sourceTerrain348_!=counted||
           counted->base_.entity_.parent1C!=&racer.racer)return false;
        //Caller supplies its actual158A70 output or actual Racer378 lane,
        //respectively. The source PC is the retained Racer cursor, not input.
        if(racer.nativePostEmberPC_==0x2148f0u)return source348Stages_[slot].writeSideForce2148F0(actualSourceValue);
        if(racer.nativePostEmberPC_==0x214918u)return source348Stages_[slot].writeGravity214918(actualSourceValue);
        return false;
    }
    bool applySource348Setup21494C(std::size_t slot,
        g::OriginalRacerEntityCold50LA& racer,g::VehicleFrameParameters49W& parameters,
        float actual324,float actualLift7EC,float actualSpring,float actualDamping,
        float actualOrientationSpring,std::string& error){
        auto* s=storage_.get();const auto pin=bankLifetime_.lock();
        if(!source348Arena_||!s||!s->sharedRoster_||!actual_||terminal_||closed_||!pin||
           !bankIdentity_||pin->identity!=bankIdentity_||bankIdentity_->raWorldLifetime_.get()!=pin.get()||
           !bankIdentity_->sealed_||bankIdentity_->retired_||bankIdentity_->visitDepth_!=1u||
           bankIdentity_->currentSlot_!=slot||slot>=source348Stages_.size()||slot>=s->sharedRoster_->count||
           !registeredRange(&parameters,sizeof(parameters))||
           !racer.prepared()||racer.nativePostEmberPC_!=0x21494cu||!racer.body340||
           racer.body340->canonicalNumericAddress()!=&actual_->frame.bodies.main)return false;
        auto& row=s->sharedRoster_->objects[slot*storage::Storage::rosterRoles+
            static_cast<std::size_t>(storage::Storage::RosterRole::terrain348)];
        if(!row||row->coldBound||row->membership.state.attached||row->membership.state.object||
           row->nativeTerrainBody128!=racer.body340||row->generation!=racer.racer.sessionGeneration||
           !row->terrainResource||row->terrainResource->initialized||!row->terrainResult90||
           row->terrainResult90->constructed_||row->terrainResult90->revoked_)return false;
        auto* counted=source348CountedOwners_[slot];
        if(!counted||!counted->appended()||counted->retired_||counted->stage_!=&source348Stages_[slot]||
           !racer.sourceTerrain348_||*racer.sourceTerrain348_!=counted||
           counted->base_.entity_.parent1C!=&racer.racer)return false;
        auto shape=[&](const g::LocalSegment48I& segment)noexcept {
            if(!counted->constructShape11C3F0(segment,*row->terrainResource,*row->terrainResult90))return false;
            //Actual caller stores after11C3F0 return, before Stage projects its
            //callback metadata and advances to localOffset1C0 arithmetic.
            return counted->bindSelfCallback1B1478();
        };
        if(!source348Stages_[slot].applySetup21494C(actual_->terrain,actual_->terrainLive,
            actual_->main,parameters,actual324,actualLift7EC,actualSpring,actualDamping,
            actualOrientationSpring,shape,error))return false;
        //Only the actual348 setter is now complete in this native component.
        //Recovery/role filters/Session/World membership remain separate joins.
        //The caller owns source loader cursor advancement, never this bridge.
        return true;
    }

    bool publishSource690Bounds2121C8(std::size_t slot,
        g::OriginalRacerEntityCold50LA& racer,g::OriginalRacerQuery690Owner50LA& query)noexcept {
        auto* s=storage_.get();const auto pin=bankLifetime_.lock();
        if(!s||!s->sharedRoster_||!actual_||terminal_||closed_||!pin||!bankIdentity_||
           pin->identity!=bankIdentity_||bankIdentity_->raWorldLifetime_.get()!=pin.get()||
           !bankIdentity_->sealed_||bankIdentity_->retired_||bankIdentity_->visitDepth_!=1u||
           bankIdentity_->currentSlot_!=slot||slot>=inputs_.collisionRoster.size()||
           slot>=s->sharedRoster_->count||!racer.prepared()||!racer.sourceQuery690_||
           *racer.sourceQuery690_!=&query||query.collision_!=inputs_.collisionRoster[slot]||
           actual_->main.resource!=&query.collision_->main()||
           query.scene190_!=racer.scene190||!racer.scene190||racer.scene190->released||
           query.base_.entity_.sessionGeneration!=racer.racer.sessionGeneration||
           query.base_.entity_.parent1C!=&racer.racer||query.base_.entity_.mask20!=0x402u||
           query.boundary_!=0x2121c0u)return false;
        const auto& row=s->sharedRoster_->objects[slot*storage::Storage::rosterRoles+
            static_cast<std::size_t>(storage::Storage::RosterRole::main690)];
        if(!row||row->slot!=slot||row->collision!=query.collision_||row->coldBound||
           row->membership.state.attached||row->membership.state.object)return false;
        return query.publishBounds2121C8(racer.sourceCollisionCenter310_,racer.sourceCollisionHalf320_);
    }

    std::weak_ptr<ra_cold_roster50LA::FreshExecutionArena> constructorArena_;
    bool registerConstructorWorldOwners(
        const std::shared_ptr<ra_cold_roster50LA::FreshExecutionArena>&,
        const std::shared_ptr<OwnershipBridge>&,Diagnostic&)noexcept;
    bool bindConstructorActual(const std::shared_ptr<ra_cold_roster50LA::FreshExecutionArena>&,
        Diagnostic&)noexcept;
    bool stageConstructorPod694(std::size_t,const g::OriginalRacerMainBodyOwner50LA&,
        const g::VehicleFrameParameters49W&)noexcept;
    bool sourceContact354Lanes212278(std::size_t,const g::OriginalRacerMainBodyOwner50LA&,
        g::SpringGeneratedParameters48K*&,g::SpringResponseParameters48L*&)noexcept;
    bool sourceContact368Lanes2127C8(std::size_t,const g::OriginalRacerSecondaryBodyOwner50LA&,
        const g::OriginalRacerEntityCold50LA&,g::SpringGeneratedParameters48K*&,g::SpringResponseParameters48L*&)noexcept;
    //Native one-way custody: real source7A4 allocations must never fall back
    //to the unrelated fixed Fresh recovery arrays, even after partial failure.
    std::array<bool,8> sourceAuxiliaryAdopted_{};
    bool adoptSourceAuxiliaryQueries2123C8(std::size_t,const g::OriginalRacerMainBodyOwner50LA&)noexcept;
    bool fixedRecoveryRouteAllowed()const noexcept {
        for(bool adopted:sourceAuxiliaryAdopted_)if(adopted)return false;
        return true;
    }

    // Retained native per-roster694/A0 custody, NOT a guest694 Entity, a
    // CollectionRef, or a simultaneous World registration. The existing
    // shared-role membership remains unchanged until its real producer joins.
    enum class NativePodResponseE8 { unpublished,constructorNull11B350 };
    struct NativePod694Owner {
        const native_racer_collision50HW::Owner* collision=nullptr;
        const g::PodShapeSource49W* shape=nullptr;
        const g::OriginalRacerMainBodyOwner50LA* bodyA0=nullptr;
        std::uint64_t generation=0;
        std::size_t slot=0;
        NativePodResponseE8 responseE8=NativePodResponseE8::unpublished;
    };
    std::array<NativePod694Owner,8> stagedPods694_{};
    std::array<std::weak_ptr<g::NativeSecondaryContactOwner50LA>,8> stagedSecondaryContacts_{};
    std::array<std::weak_ptr<g::NativeRacerTerrainOwner50LA>,8> stagedRacerTerrain_{};
    std::array<std::weak_ptr<g::NativeRacerAeroOwner50LA>,8> stagedAero_{};
    //Private native identity admission after the real main Registry producer,
    //before any shared source query invocation. This performs no source body,
    //scene or geometry callback and does not activate the shared factory.
    bool registerNativeSecondaryContactRegistry(native_dynamic_contact50IW::Registry& registry)noexcept {
        const auto pin=bankLifetime_.lock();const auto* producer=endCandidateProducer_;
        if(!producer||producer->inCall||producer->owner.lock().get()!=this||
           producer->storage!=storage_.get()||producer->bodyRegistry!=&registry||
           !registry.prepared()||!pin||!bankIdentity_||pin->identity!=bankIdentity_||terminal_||closed_||
           !actual_||!storage_.get()||!storage_.get()->sharedRoster_||storage_.get()->sharedRoster_->retired||
           !bankIdentity_->sealed_||bankIdentity_->retired_||bankIdentity_->visiting_||
           bankIdentity_->readBlocked_||bankIdentity_->visitDepth_||bankIdentity_->sourceQueryPending()||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||
           registry.count()!=bankIdentity_->count_||
           registry.count()!=storage_.get()->sharedRoster_->count||
           !registeredRange(&actual_->frame.bodies.secondary,sizeof(actual_->frame.bodies.secondary))||
           !registeredRange(actual_->frame.secondaryContactPartners210,
               sizeof(actual_->frame.secondaryContactPartners210)))return false;
        std::array<std::shared_ptr<g::NativeSecondaryContactOwner50LA>,8> owners{};
        for(std::size_t slot=0;slot<registry.count();++slot){
            owners[slot]=stagedSecondaryContacts_[slot].lock();
            const auto& owner=owners[slot];
            if(!rosterSlotComplete_[slot]||!owner||owner->revoked_||owner->slot_!=slot||
               owner->bank_!=bankIdentity_||owner->frame_!=&actual_->frame||!owner->parameters_||
               owner->generation_!=stagedPods694_[slot].generation||
               !registeredRange(owner->parameters_,sizeof(*owner->parameters_)))return false;
        }
        if(registry.secondaryPrepared_){
            for(std::size_t slot=0;slot<registry.count();++slot)
                if(!registry.nativeSecondaryIdentity(slot,*owners[slot]).value)return false;
            return true;
        }
        return registry.prepareNativeSecondary(owners.data(),registry.count());
    }
    bool nativeRacerTerrainCurrent(const original_camera_session50KX::Session& expected,
        std::size_t slot,const g::NativeRacerTerrainOwner50LA& owner)const noexcept {
        const auto pin=bankLifetime_.lock();
        if(!pin||!actual_||!bankIdentity_||pin->identity!=bankIdentity_||&expected.actual_.bank!=bankIdentity_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||terminal_||closed_||
           !camera_||camera_->session.get()!=&expected||camera_->generation!=expected.generation_||
           expected.closed_||expected.abandoned_||!expected.coldBound_||
           !expected.actual_.actualChariotModels||slot>=bankIdentity_->count_||
           slot>=expected.racerEntities_.size()||!rosterSlotComplete_[slot]||
           !owner.published_||owner.revoked_||owner.bank_!=bankIdentity_||owner.frame_!=&actual_->frame||
           owner.slot_!=slot||owner.generation_!=expected.generation_||
           owner.model_!=(*expected.actual_.actualChariotModels)[slot].get())return false;
        const auto& racer=expected.racerEntities_[slot];
        return owner.racer_==&racer&&racer.prepared()&&racer.racer.reference00&&
            racer.racer.kind==g::OriginalWreckComponentKind50LA::racer27E130&&
            racer.racer.sessionGeneration==owner.generation_&&racer.racer.objectKey==owner.racerKey_&&
            owner.racerKey_&&racer.body340==stagedPods694_[slot].bodyA0&&racer.body340&&
            racer.body340->canonicalNumericAddress()==&actual_->frame.bodies.main&&
            racer.body340->scene288()==racer.scene190;
    }
    bool registerNativeRacerTerrainRegistry(const original_camera_session50KX::Session& expected,
        native_dynamic_contact50IW::Registry& registry)noexcept {
        const auto* producer=endCandidateProducer_;
        if(!producer||producer->inCall||producer->owner.lock().get()!=this||
           producer->storage!=storage_.get()||producer->bodyRegistry!=&registry||
           !storage_.get()||!storage_.get()->sharedRoster_||storage_.get()->sharedRoster_->retired||
           !registry.prepared()||!registry.secondaryPrepared_||!bankIdentity_||
           !bankIdentity_->sealed_||bankIdentity_->retired_||bankIdentity_->visiting_||bankIdentity_->readBlocked_||
           bankIdentity_->visitDepth_||bankIdentity_->sourceQueryPending()||
           registry.count()!=bankIdentity_->count_||registry.count()!=storage_.get()->sharedRoster_->count)return false;
        std::array<std::shared_ptr<g::NativeRacerTerrainOwner50LA>,8> owners{};
        for(std::size_t slot=0;slot<registry.count();++slot){
            owners[slot]=stagedRacerTerrain_[slot].lock();
            if(!owners[slot]||!nativeRacerTerrainCurrent(expected,slot,*owners[slot]))return false;
        }
        if(registry.racerTerrainPrepared_){
            for(std::size_t slot=0;slot<registry.count();++slot)
                if(!registry.racerTerrainIdentity(slot,*owners[slot]).value)return false;
            return true;
        }
        return registry.prepareRacerTerrainOwners(owners.data(),registry.count());
    }
    bool stageNativePod694(std::size_t slot,const g::OriginalRacerMainBodyOwner50LA& body,
        const g::VehicleFrameParameters49W& parameters)noexcept {
        const auto pin=bankLifetime_.lock();
        if(!pin||pin->identity!=bankIdentity_||!bankIdentity_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||terminal_||closed_||
           !bankIdentity_->sealed_||bankIdentity_->retired_||bankIdentity_->visitDepth_!=1u||
           bankIdentity_->currentSlot_!=slot||slot>=inputs_.collisionRoster.size()||
           stagedPods694_[slot].bodyA0||!actual_||!actual_->recovery||
           !actual_->recovery->sourcePrepared||
           !registeredRange(&parameters.shape,sizeof(parameters.shape)))return false;
        const auto* collision=inputs_.collisionRoster[slot];
        const auto& entity=body.entity();
        if(!collision||!collision->prepared()||bodyIdentities_[slot].collision!=collision||
           actual_->recovery->selectedSource!=collision||
           actual_->main.resource!=&collision->main()||actual_->secondary.resource!=&collision->secondary()||
           actual_->mainFilter.identity!=bodyIdentities_[slot].main||
           actual_->secondaryFilter.identity!=bodyIdentities_[slot].secondary||
           body.canonicalNumericAddress()!=&actual_->frame.bodies.main||!body.scene288()||
           !body.scene288()->constructed||body.scene288()->released||!entity.reference00||
           !entity.objectKey||
           entity.kind!=g::OriginalWreckComponentKind50LA::body167920||!entity.sessionGeneration||
           body.scene288()->entity().sessionGeneration!=entity.sessionGeneration||
           !registeredRange(&actual_->frame.shape,sizeof(actual_->frame.shape))||
           !registeredRange(&actual_->frame.bodies.main.components.body20C,sizeof(std::uint32_t))||
           !registeredRange(actual_->frame.mainContactPartners210,sizeof(actual_->frame.mainContactPartners210)))return false;
        const auto& shape=collision->mainTarget().source;
        const auto& authored=parameters.shape;
        if(std::memcmp(shape.local40.v,authored.local40.v,sizeof(shape.local40.v))||
           std::memcmp(shape.rootCenter.v,authored.rootCenter.v,sizeof(shape.rootCenter.v))||
           std::memcmp(shape.rootHalf.v,authored.rootHalf.v,sizeof(shape.rootHalf.v)))return false;
        //The native fresh694 owner now retains the genuine constructor's
        //response lane:2121F8 calls11B190, whose11B350 stores nullE8.
        //No mutable response registration is exposed for this native owner.
        //Unpublished/revoked state is distinct and cannot take the null branch.
        stagedPods694_[slot]={collision,&shape,&body,entity.sessionGeneration,slot,
            NativePodResponseE8::constructorNull11B350};return true;
    }
    bool revokeNativePod694(std::size_t slot,native_racer50HN::Bank& bank)noexcept {
        //Withdraw active AND saved Bank resource borrows while actual body and
        //scene still live, before the legacy shared owner/token revocations.
        if(!retireNativeRepulsorPayload(slot,bank))return false;
        revokeSharedColdOwners(slot);
        if(slot<stagedAero_.size()){
            if(auto owner=stagedAero_[slot].lock())owner->revoked_=true;
            stagedAero_[slot].reset();
        }
        if(slot<stagedRacerTerrain_.size()){
            if(auto owner=stagedRacerTerrain_[slot].lock())owner->revoked_=true;
            stagedRacerTerrain_[slot].reset();
        }
        if(slot<stagedPods694_.size()){
            if(const auto owner=stagedSecondaryContacts_[slot].lock())owner->revoked_=true;
            stagedSecondaryContacts_[slot].reset();stagedPods694_[slot]={};
        }
        return true;
    }
    // Shared-mode opaque identities are assigned only at the actual selected
    // resource constructor cut. Every destination must already belong to the
    // current sealed Bank; no immutable legacy namespace is changed.
    bool assignSharedColdIdentities(std::size_t slot)noexcept {
        auto* s=storage_.get();if(!s||!s->sharedRoster_)return true;
        const auto pin=bankLifetime_.lock();
        if(!pin||!bankIdentity_||pin->identity!=bankIdentity_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||!bankIdentity_->sealed_||
           bankIdentity_->retired_||bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           slot>=inputs_.collisionRoster.size()||terminal_||closed_||!actual_||
           !actual_->recovery||!actual_->movement)return false;
        auto& q=*actual_;auto& r=*q.recovery;const auto& ids=bodyIdentities_[slot];
        if(!ids.terrain||!ids.pod||!ids.movement||ids.collision!=inputs_.collisionRoster[slot]||
           q.main.resource!=&ids.collision->main()||q.secondary.resource!=&ids.collision->secondary()||
           q.mainFilter.identity!=ids.main||q.secondaryFilter.identity!=ids.secondary||
           r.sourcePrepared||!r.storagePrepared)return false;
        for(const auto* lane:{&q.terrain.metadata.filter.identity,&r.globals.terrainIdentity348,
            &r.globals.identity[0],&r.globals.identity[1],&r.podFilter.identity,&q.movement->filter.identity})
            if(!registeredRange(lane,sizeof(*lane)))return false;
        q.terrain.metadata.filter.identity=ids.terrain;r.globals.terrainIdentity348=ids.terrain;
        r.globals.identity[0]=r.podFilter.identity=ids.pod;
        r.globals.identity[1]=q.movement->filter.identity=ids.movement;
        return true;
    }
    bool constructSharedTerrainResource(std::size_t slot,
        const terrain_probe_authoring50fq_fixture::Owner& authored)noexcept {
        auto* s=storage_.get();if(!s||!s->sharedRoster_)return true;
        const auto pin=bankLifetime_.lock();
        if(!pin||!bankIdentity_||pin->identity!=bankIdentity_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||!bankIdentity_->sealed_||
           bankIdentity_->retired_||bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           slot>=inputs_.collisionRoster.size()||terminal_||closed_||!actual_||
           !authored.prepared||!actual_->terrain.prepared||
           !registeredRange(&actual_->terrain.initial,sizeof(actual_->terrain.initial))||
           !live_terrain_probe50fr::matches(actual_->terrainLive,actual_->terrain.initial))return false;
        auto& row=s->sharedRoster_->objects[slot*storage::Storage::rosterRoles+
            static_cast<std::size_t>(storage::Storage::RosterRole::terrain348)];
        const auto& ids=bodyIdentities_[slot];const auto& installed=actual_->terrain.initial.segment;
        if(!row||row->collision!=ids.collision||row->slot!=slot||row->coldBound||
           !row->terrainResource||row->terrainResource->initialized||
           !row->terrainResult90||row->terrainResult90->constructed_||row->terrainResult90->revoked_||
           actual_->main.resource!=&ids.collision->main()||
           authored.metadata.filter.identity!=ids.terrain||actual_->terrain.metadata.filter.identity!=ids.terrain||
           std::memcmp(installed.first.v,authored.initial.segment.first.v,16)||
           std::memcmp(installed.second.v,authored.initial.segment.second.v,16)||
           std::memcmp(installed.worldMatrix.v,authored.initial.segment.worldMatrix.v,64))return false;
        //Genuine immutable resource constructor immediately after the actual
        //cold348 install, from the still-live authored segment, NOT a saved
        //frame snapshot. rays points into this retained row's own pair.
        if(!live_terrain_probe50fr::initialize(*row->terrainResource,authored.initial.segment))return false;
        //Same actual one-pair cold producer:11C5B4..11C6E4 constructs a
        //SEPARATE result90, before later11DD00 reset. Retained unbanked per-row
        //storage is already charged by sizeof to the unchanged native budget.
        return row->terrainResult90->construct();
    }
    std::array<bool,storage::Storage::maximumRoster> sharedFilterAttempted_{};
    bool constructSharedColdFilters(std::size_t slot,
        const std::array<storage::Storage::SharedRosterObject*,storage::Storage::rosterRoles>& rows)noexcept {
        auto& allocation=*storage_.get()->sharedRoster_;
        if(slot>=allocation.count||sharedFilterAttempted_[slot])return false;
        //All actual scalar/list schema is still at the same successful cold
        //constructor cut. This is not a later per-frame snapshot producer.
        const auto& q=*actual_;const g::CollisionFilter48J* filters[]={&q.mainFilter,&q.secondaryFilter,
            &q.terrain.metadata.filter,&q.recovery->podFilter,&q.movement->filter};
        const std::uint32_t expected0[]={q.secondaryFilter.identity,q.recovery->podFilter.identity,
            q.movement->filter.identity,q.terrain.metadata.filter.identity};
        const std::uint32_t expected1[]={q.mainFilter.identity,q.recovery->podFilter.identity,q.movement->filter.identity};
        const std::uint32_t expected3[]={q.mainFilter.identity,q.movement->filter.identity,q.terrain.metadata.filter.identity};
        const std::uint32_t* expected[]={expected0,expected1,nullptr,expected3,nullptr};
        const std::size_t sizes[]={4,3,0,3,0};
        std::size_t payload=0;
        for(std::size_t i=0;i<rows.size();++i){
            const auto& f=*filters[i];const auto& first=f.identityLists[0];
            if(!rows[i]||rows[i]->coldFilter||rows[i]->coldBound||first.identities.size()!=sizes[i]||
               first.mode!=1||f.identityLists[1].mode!=1||!f.identityLists[1].identities.empty())return false;
            for(std::size_t n=0;n<sizes[i];++n)if(first.identities[n]!=expected[i][n])return false;
            std::size_t bytes=sizeof(storage::Storage::SharedColdFilter);
            for(const auto n:{first.identities.size(),f.identityLists[1].identities.size(),f.categoryMasks.size()}){
                if(n>static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max())||
                   n>(std::numeric_limits<std::size_t>::max()-bytes)/sizeof(std::uint32_t))return false;
                bytes+=n*sizeof(std::uint32_t);
            }
            if(bytes>allocation.coldPayloadRemaining||payload>allocation.coldPayloadRemaining-bytes)return false;
            payload+=bytes;
        }
        //Reserve exact actual payload before allocation; partial construction
        //retains its reservation and terminal attempt, never retries/duplicates.
        sharedFilterAttempted_[slot]=true;allocation.coldPayloadRemaining-=payload;
        try {
            for(std::size_t i=0;i<rows.size();++i)
                rows[i]->coldFilter=std::make_unique<const storage::Storage::SharedColdFilter>(*filters[i]);
            return true;
        }catch(...){return false;}
    }
    bool bindSharedColdOwners(std::size_t slot,
        const terrain_probe_authoring50fq_fixture::Owner& authored)noexcept {
        auto* s=storage_.get();if(!s||!s->sharedRoster_)return true;
        const auto pin=bankLifetime_.lock();
        if(!pin||!bankIdentity_||pin->identity!=bankIdentity_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||!bankIdentity_->sealed_||
           bankIdentity_->retired_||bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           slot>=inputs_.collisionRoster.size()||!actual_||!actual_->recovery||!actual_->movement||
           terminal_||closed_||s->sharedRoster_->retired||!authored.prepared)return false;
        auto& q=*actual_;const auto& ids=bodyIdentities_[slot];const auto& staged=stagedPods694_[slot];
        if(!staged.bodyA0||staged.collision!=ids.collision||!staged.generation||
           q.recovery->selectedSource!=ids.collision||!q.recovery->sourcePrepared||
           q.main.resource!=&ids.collision->main()||q.secondary.resource!=&ids.collision->secondary()||
           !q.terrain.prepared||!live_terrain_probe50fr::matches(q.terrainLive,q.terrain.initial)||
           std::memcmp(q.terrain.initial.segment.first.v,authored.initial.segment.first.v,16)||
           std::memcmp(q.terrain.initial.segment.second.v,authored.initial.segment.second.v,16)||
           std::memcmp(q.terrain.initial.segment.worldMatrix.v,authored.initial.segment.worldMatrix.v,64))return false;
        const std::uint32_t identities[]={ids.main,ids.secondary,ids.terrain,ids.pod,ids.movement};
        g::CollisionFilter48J* filters[]={&q.mainFilter,&q.secondaryFilter,&q.terrain.metadata.filter,
            &q.recovery->podFilter,&q.movement->filter};
        std::array<storage::Storage::SharedRosterObject*,storage::Storage::rosterRoles> rows{};
        for(std::size_t i=0;i<rows.size();++i){
            auto& row=s->sharedRoster_->objects[slot*rows.size()+i];
            if(!row||row->slot!=slot||static_cast<std::size_t>(row->role)!=i||
               row->collision!=ids.collision||row->coldBound||row->identity||
               row->membership.state.attached||row->membership.state.object||
               !identities[i]||filters[i]->identity!=identities[i])return false;
            for(std::size_t j=0;j<i;++j)if(identities[i]==identities[j])return false;
            rows[i]=row.get();
        }
        auto& terrain=*rows[static_cast<std::size_t>(storage::Storage::RosterRole::terrain348)];
        if(!terrain.terrainResource||!terrain.terrainResource->initialized||
           !terrain.terrainResult90||!terrain.terrainResult90->constructed_||terrain.terrainResult90->revoked_||
           !live_terrain_probe50fr::matches(*terrain.terrainResource,authored.initial)||
           authored.metadata.filter.identity!=ids.terrain)return false;
        if(!constructSharedColdFilters(slot,rows))return false;
        //Reuse the exact earlier producer's retained identity; never rebase
        //this to arena.terrainLive or initialize it again during observation.
        for(std::size_t i=0;i<rows.size();++i){
            auto& row=*rows[i];row.identity=identities[i];row.canonicalFilter=filters[i];
            row.generation=staged.generation;row.coldBound=true;
        }
        rows[0]->type2Resource=&ids.collision->main();rows[1]->type2Resource=&ids.collision->secondary();
        terrain.type2Resource=&terrain.terrainResource->resource;
        //21207C/1B11FC binds348+128 to this genuine main340 owner. Native
        //cold custody is delayed here, not a replay of Entity construction.
        //No World3FC/400 auxiliary pointer is inferred from this positive bind.
        terrain.nativeTerrainBody128=staged.bodyA0;
        rows[3]->type0Resource=staged.shape;rows[3]->bodyA0=staged.bodyA0;
        //7A4 endpoints/metadata are a later actual FreshRecovery producer.
        //No shape, attachment, query publication or revision is invented here.
        return true;
    }
    void revokeSharedColdOwners(std::size_t slot)noexcept {
        auto* s=storage_.get();if(!s||!s->sharedRoster_||slot>=s->sharedRoster_->count)return;
        for(std::size_t i=0;i<storage::Storage::rosterRoles;++i){
            auto& row=s->sharedRoster_->objects[slot*storage::Storage::rosterRoles+i];
            if(!row)continue;
            row->canonicalFilter=nullptr;row->bodyA0=nullptr;
            row->nativeTerrainBody128=nullptr;row->coldBound=false;
            if(row->terrainResult90)row->terrainResult90->revoke();
            //Immutable resource backing stays retained until actual World
            //retirement. No membership removal or live source zeroing occurs.
        }
    }
    //Private Stage C registration seam, still uncalled. It does not issue a
    //Bank pin or query view; later shared registration must prove all shared
    //topology/membership spans absent and enroll the remaining real owners.
    bool sharedAuxiliaryRegistrationAttempted_=false;
    bool registerSharedAuxiliaryIdentitySpans(native_racer50HN::Bank& bank,
        const ActualQueryOwners& a)noexcept {
        auto* s=storage_.get();if(!s||!s->sharedRoster_||s->sharedRoster_->retired||
           terminal_||closed_||sharedAuxiliaryRegistrationAttempted_||bank.sealed_||bank.retired_||bank.visiting_||bank.readBlocked_||bankIdentity_!=&bank||
           bankLifetime_.expired()||bank.raWorldLifetime_.get()!=bankLifetime_.lock().get()||!a.recovery||a.recovery!=inputs_.recovery||
           &a.terrain!=rosterTerrain_||a.movement!=rosterMovement_||!a.movement)return false;
        sharedAuxiliaryRegistrationAttempted_=true;
        try {
            bank.owner(a.terrain.metadata.filter.identity);bank.owner(a.recovery->globals.terrainIdentity348);
            bank.owner(a.recovery->globals.identity);bank.owner(a.recovery->podFilter.identity);
            bank.owner(a.movement->filter.identity);return true;
        }catch(...){terminal_=true;return false;} //Partial real registration is not replayable.
    }

    enum class NativeBodyListCall { contact98,processedA0,containsA4 };
    struct NativeBodyEndpoint {
        const NativePod694Owner* main=nullptr;
        std::shared_ptr<g::NativeSecondaryContactOwner50LA> secondary;
        bool typed()const noexcept{return (main!=nullptr)!=static_cast<bool>(secondary);}
        std::size_t slot()const noexcept{return main?main->slot:secondary->slot_;}
    };
    //Two explicit body kinds, never a secondary token interpreted as main340.
    //Both reuse the original algorithms but keep20C/210 and238/23C separate.
    //Secondary recipients are only the genuinely active source primary. A
    //different-slot secondary has no caller here and is deliberately refused.
    bool callNativeBodyList(const NativeBodyEndpoint& recipient,
        const NativeBodyEndpoint& other,NativeBodyListCall call,
        g::BodyContactListStatus48R& status,bool& found)noexcept {
        if(!recipient.typed()||!other.typed()||(recipient.secondary&&other.secondary))return false;
        auto* producer=endCandidateProducer_;
        const auto pin=bankLifetime_.lock();
        if(!producer||!producer->inCall||!producer->activeRevision||!producer->activeSession||
           producer->owner.lock().get()!=this||producer->storage!=storage_.get()||
           !pin||!bankIdentity_||pin->identity!=bankIdentity_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||terminal_||closed_)return false;
        const auto& session=*producer->activeSession;
        auto& bank=session.actual_.bank; //Actual mutable owner, not const identity pointer.
        if(&bank!=bankIdentity_)return false;
        if(!bank.sealed_||bank.retired_||bank.visitDepth_!=1u||(!bank.visiting_&&!bank.readBlocked_)||
           producer->activeSlot>=bank.count_||
           bank.currentSlot_!=producer->activeSlot||bank.count_!=inputs_.collisionRoster.size()||
           !camera_||camera_->session.get()!=&session||&session.actual_.bank!=&bank||
           session.closed_||session.abandoned_||!session.coldBound_||!session.readsBound_||
           !actual_||!gridArena_||&session.actual_.arena!=&actual_->frame||
           session.generation_!=camera_->generation||!session.actual_.applicationEntities||
           (recipient.slot()!=producer->activeSlot&&other.slot()!=producer->activeSlot)||
           !producer->bodyRegistry||!producer->bodyRegistry->prepared()||
           producer->bodyRegistry->count()!=bank.count_)return false;
        const auto currentMain=[&](const NativePod694Owner& row){
            if(row.slot>=bank.count_||&row!=&stagedPods694_[row.slot]||!bank.slots_[row.slot].active||
               !rosterSlotComplete_[row.slot]||!row.bodyA0||!row.collision||!row.shape||
               row.collision!=inputs_.collisionRoster[row.slot]||
               row.collision!=bodyIdentities_[row.slot].collision||
               row.shape!=&row.collision->mainTarget().source)return false;
            const auto& racer=session.racerEntities_[row.slot];const auto& entity=row.bodyA0->entity();
            return racer.prepared()&&racer.body340==row.bodyA0&&racer.scene190==row.bodyA0->scene288()&&
                racer.racer.objectKey==session.racerKey(row.slot)&&
                racer.racer.sessionGeneration==session.generation_&&row.generation==session.generation_&&
                entity.sessionGeneration==row.generation&&entity.reference00&&
                entity.kind==g::OriginalWreckComponentKind50LA::body167920&&entity.objectKey&&
                racer.scene190&&racer.scene190->constructed&&!racer.scene190->released&&
                racer.actualCounter423F60()==&session.actual_.applicationEntities->objectCount423F60()&&
                racer.actualLock423F58()==&session.actual_.applicationEntities->attachmentLock423F58()&&
                entity.parent1C==&racer.racer&&entity.mask20==2u&&
                row.bodyA0->canonicalNumericAddress()==&actual_->frame.bodies.main&&
                producer->bodyRegistry->bodyIdentityForSlot(row.slot).value;
        };
        const auto current=[&](const NativeBodyEndpoint& endpoint){
            if(endpoint.main)return currentMain(*endpoint.main);
            const auto& owner=*endpoint.secondary;const auto slot=owner.slot_;
            const auto retained=slot<stagedSecondaryContacts_.size()?
                stagedSecondaryContacts_[slot].lock():nullptr;
            if(!retained||retained.get()!=&owner||owner.revoked_||slot!=producer->activeSlot||
               !rosterSlotComplete_[slot]||!bank.slots_[slot].active||
               owner.bank_!=&bank||owner.frame_!=&actual_->frame||
               owner.generation_!=session.generation_||!owner.parameters_||
               !registeredRange(owner.parameters_,sizeof(*owner.parameters_))||
               !registeredRange(&actual_->frame.bodies.secondary,sizeof(actual_->frame.bodies.secondary))||
               !registeredRange(actual_->frame.secondaryContactPartners210,
                   sizeof(actual_->frame.secondaryContactPartners210))||
               !gridArena_->rigs[slot]||gridArena_->rigs[slot].get()!=gridRigIdentity_[slot]||
               !actual_->frame.bodies.main.domain.secondBodyPresent||
               owner.physical19C_!=gridArena_->bindings.lifecycle.physical19C||
               owner.physical19C_!=gridArena_->rigs[slot]->physical||
               static_cast<std::size_t>(owner.physical19C_)>=actual_->frame.sceneNodeCount||
               !bodyIdentities_[slot].collision||
               actual_->secondaryFilter.identity!=bodyIdentities_[slot].secondary||
               actual_->secondary.resource!=&bodyIdentities_[slot].collision->secondary())return false;
            return producer->bodyRegistry->nativeSecondaryIdentity(slot,owner).value!=0;
        };
        if(!current(recipient)||!current(other)||
           actual_->mainFilter.identity!=bodyIdentities_[producer->activeSlot].main||
           actual_->main.resource!=&bodyIdentities_[producer->activeSlot].collision->main())return false;
        // Source numeric backing is shared only while visiting. A different
        // slot is the SAME Bank's saved owner span, not the current arena and
        // not a copied/fresh physics body. Slot0 was saved before nonzero visits.
        const auto backing=[&](void* address,std::size_t bytes)->unsigned char* {
            const auto first=reinterpret_cast<std::uintptr_t>(address);
            for(const auto& span:bank.spans_){
                const auto begin=reinterpret_cast<std::uintptr_t>(span.address);
                if(first<begin||first-begin>span.bytes||bytes>span.bytes-(first-begin))continue;
                if(recipient.slot()==producer->activeSlot)return static_cast<unsigned char*>(address);
                const auto offset=span.offset+(first-begin);
                auto& saved=bank.slots_[recipient.slot()].bytes;
                if(offset>saved.size()||bytes>saved.size()-offset)return nullptr;
                return saved.data()+offset;
            }
            return nullptr;
        };
        auto& frame=actual_->frame;
        auto& root=recipient.secondary?frame.bodies.secondary.root:frame.bodies.main.components.root;
        auto& contactCount=recipient.secondary?frame.bodies.secondary.body20C:frame.bodies.main.components.body20C;
        auto* contactSlots=recipient.secondary?frame.secondaryContactPartners210:frame.mainContactPartners210;
        const bool processed=call!=NativeBodyListCall::contact98;
        auto* count=backing(processed?static_cast<void*>(&root.callbackState238):
            static_cast<void*>(&contactCount),sizeof(std::uint32_t));
        auto* partners=backing(processed?static_cast<void*>(root.processedContactPartners23C):
            static_cast<void*>(contactSlots),sizeof(frame.mainContactPartners210));
        if(!count||!partners)return false;
        g::BodyContactListState48R state;
        std::memcpy(&state.count20C,count,sizeof(state.count20C));
        const auto identity=other.secondary?
            producer->bodyRegistry->nativeSecondaryIdentity(other.slot(),*other.secondary):
            producer->bodyRegistry->bodyIdentityForSlot(other.slot());
        if(!identity.value)return false;
        if(call==NativeBodyListCall::containsA4){
            //165540 scans every entry for signed count>0 (no early exit).
            //Negative/zero performs no load. Above ten is not bounded backing.
            if(state.count20C>static_cast<std::int32_t>(g::BodyContactCapacity48R))return false;
            bool match=false;
            for(std::int32_t i=0;i<state.count20C;++i){
                g::BodyContactIdentity48R item;
                std::memcpy(&item,partners+static_cast<std::size_t>(i)*sizeof(item),sizeof(item));
                if(item.value==identity.value)match=true;
            }
            found=match;return true;
        }
        std::memcpy(state.slots210.data(),partners,sizeof(frame.mainContactPartners210));
        const auto before=state.count20C;
        //1654C0 uses the same signed capacity/duplicate/append arithmetic as
        //165430, but its separately authenticated238/23C lanes above. Reuse
        //only the numerical append leaf, never the +98 storage or reset.
        status=g::applyBodyContactList48R(identity,state);
        if(status==g::BodyContactListStatus48R::InvalidInput)return false;
        if(status==g::BodyContactListStatus48R::Appended){
            //165498/165528 write the new slot, then1654A4/165534 count+1. Preserve
            //all untouched raw slots, including unknown constructor suffixes.
            std::memcpy(partners+static_cast<std::size_t>(before)*sizeof(g::BodyContactIdentity48R),
                &state.slots210[static_cast<std::size_t>(before)],sizeof(g::BodyContactIdentity48R));
            std::memcpy(count,&state.count20C,sizeof(state.count20C));
        }
        return true;
    }

    //Preserve every existing main-only call without widening its source
    //owner or storage domain. Native endpoint refs have this lexical lifetime.
    bool callNativeStagedBodyList(const NativePod694Owner& recipient,
        const NativePod694Owner& other,NativeBodyListCall call,
        g::BodyContactListStatus48R& status,bool& found)noexcept {
        return callNativeBodyList({&recipient,{}},{&other,{}},call,status,found);
    }

    bool applyNativeStagedBodyContact98(const NativePod694Owner& recipient,
        const NativePod694Owner& other,g::BodyContactListStatus48R& status)noexcept {
        bool unused=false;
        return callNativeStagedBodyList(recipient,other,NativeBodyListCall::contact98,status,unused);
    }
    // Private reached subphase of294BB0, starting precisely at294C34.
    // Caller must already publish the actual geometry/hit/copies and earlier
    // currentCandidate130 effect. This is NOT an E4 completion or geometry
    // receipt issuer. Incomplete results retain all earlier writes; caller
    // must stop that invocation, not replay this phase with a new projection.
    enum class NativeDynamicBodyCut {
        primaryLookup294C44,otherProcessed294C68,primaryContact294C7C,
        otherContact294C90,material294D04,pairRecords294DD4
    };
    struct NativeDynamicBodyPhase {
        NativeDynamicBodyCut cut=NativeDynamicBodyCut::primaryLookup294C44;
        bool suppressPairRecords=false;
        std::uint32_t material=0;
    };
    NativeDynamicBodyPhase applyNativeDynamicBodyPhase(
        const NativeBodyEndpoint& primary,const NativePod694Owner& other)noexcept {
        NativeDynamicBodyPhase out;
        //Query A0 is the actual visited typed body; target remains POD A0/main340.
        const auto* producer=endCandidateProducer_;
        if(!producer||!primary.typed()||primary.slot()!=producer->activeSlot)return out;
        const NativeBodyEndpoint target{&other,{}};
        g::BodyContactListStatus48R status{};bool found=false;
        if(!callNativeBodyList(primary,target,NativeBodyListCall::containsA4,status,found))return out;
        out.suppressPairRecords=found;
        if(!found){
            out.cut=NativeDynamicBodyCut::otherProcessed294C68;
            if(!callNativeBodyList(target,primary,NativeBodyListCall::processedA0,status,found))return out;
            out.cut=NativeDynamicBodyCut::primaryContact294C7C;
            if(!callNativeBodyList(primary,target,NativeBodyListCall::contact98,status,found))return out;
            out.cut=NativeDynamicBodyCut::otherContact294C90;
            if(!callNativeBodyList(target,primary,NativeBodyListCall::contact98,status,found))return out;
        }
        // Original reads OTHER body.scene288, not shape owner9C. Existing
        // main-body authentication proved this live, constructed scene. No
        // source-null scene branch is inferred from unpublished state.
        out.cut=NativeDynamicBodyCut::material294D04;
        const auto* scene=other.bodyA0->scene288();
        if(!scene||!scene->constructed||scene->released)return out;
        const auto& name=scene->name38;
        std::size_t end=0;
        while(end<name.size()&&name[end]!='\0'&&name[end]!='.')++end;
        // Original reads three bytes before first dot/string end. A shorter
        // owner name reaches an unsafe source read: retain callbacks and stop.
        if(end<3u)return out;
        if(name[end-3u]=='f'&&name[end-2u]=='r'){
            const auto byte=static_cast<unsigned char>(name[end-1u]);
            const std::int32_t signedByte=byte<128u?byte:static_cast<std::int32_t>(byte)-256;
            out.material=static_cast<std::uint32_t>(signedByte-48);
        }
        out.cut=NativeDynamicBodyCut::pairRecords294DD4;
        return out; //Actual pair appends/E8 are later, separately ordered calls.
    }

    NativeDynamicBodyPhase applyNativeDynamicMainBodyPhase(
        const NativePod694Owner& primary,const NativePod694Owner& other)noexcept {
        return applyNativeDynamicBodyPhase({&primary,{}},other);
    }
    NativeDynamicBodyPhase applyNativeDynamicSecondaryBodyPhase(
        const NativePod694Owner& other)noexcept {
        const auto* producer=endCandidateProducer_;
        if(!producer||producer->activeSlot>=stagedSecondaryContacts_.size())return {};
        const auto primary=stagedSecondaryContacts_[producer->activeSlot].lock();
        if(!primary)return {};
        return applyNativeDynamicBodyPhase({nullptr,primary},other);
    }

    bool rosterRetirementRequired_=false;
    std::array<bool,8> rosterSlotComplete_{};
    bool cameraRetiredForRoster()const noexcept {
        return !camera_||!camera_->session||camera_->session->closed_;
    }
    friend class ra_static_lighting50LA::Binding;
    friend class ra_static_fog50LA::Binding;
    // Identity is issued only by this actual retained camera owner.
    struct StaticLightPublication {
        const original_camera_session50KX::Session* session=nullptr;
        std::uint64_t generation=0,publication=0,frame=0;
    };
    bool readStaticLightPublication(StaticLightPublication& out)const noexcept {
        if(terminal_||closed_||cameraPrepareTerminal_||!cameraBankCurrent()||
           !camera_||!camera_->session||!freshLighting_||camera_->lighting!=freshLighting_)return false;
        const auto& s=*camera_->session;
        if(!s.coldBound_||s.closed_||s.abandoned_||
           s.frameStage_!=original_camera_session50KX::Session::FrameStage::published||
           !s.publicationCount_||!s.generation_||s.generation_!=camera_->generation)return false;
        out={&s,s.generation_,s.publicationCount_,s.completedFrame_};return true;
    }
    struct StaticFogPublication {
        StaticLightPublication camera;
        const g::OriginalRaceFogFields* fog=nullptr;
        std::uint64_t revision=0;
    };
    bool readStaticFogPublication(StaticFogPublication& id,
        ra_static_fog50LA::CameraObservation& out)const noexcept {
        StaticLightPublication base;
        if(!readStaticLightPublication(base)||!camera_->fog.current()||!camera_->display||
           camera_->display->trackFog3EC!=&camera_->fog.fields_||
           camera_->session->displayInputs_!=camera_->display)return false;
        original_camera_session50KX::Session::FogPreparation current;
        if(!camera_->session->readFogPreparation(current)||current.owner!=&camera_->fog.fields_)return false;
        ra_static_fog50LA::CameraObservation next;
        next.generation=current.generation;next.publication=current.publication;
        next.frame=current.frame;next.fogRevision=current.revision;next.source=current.source;
        next.camera=current.camera;next.position=current.position;
        next.screen=current.screen;next.guardProjection=current.guardProjection;
        id={base,current.owner,current.revision};out=next;return true;
    }
    StaticFogPublication nativeStaticDrawIdentity_{};
    std::vector<g::original_gc_gameplay_lighting_candidate::NativeRaStaticDraw50LA> nativeStaticDraw_;
    bool nativeStaticDrawPrepared_=false;
    const std::vector<g::original_gc_gameplay_lighting_candidate::NativeRaStaticDraw50LA>*
    nativeStaticDraw()const noexcept {
        StaticFogPublication now;ra_static_fog50LA::CameraObservation observation;
        if(!nativeStaticDrawPrepared_||!readStaticFogPublication(now,observation)||
           now.camera.session!=nativeStaticDrawIdentity_.camera.session||
           now.camera.generation!=nativeStaticDrawIdentity_.camera.generation||
           now.camera.publication!=nativeStaticDrawIdentity_.camera.publication||
           now.camera.frame!=nativeStaticDrawIdentity_.camera.frame||
           now.fog!=nativeStaticDrawIdentity_.fog||now.revision!=nativeStaticDrawIdentity_.revision)return nullptr;
        return &nativeStaticDraw_;
    }
    // +110 adopts the already-populated native CameraBinding fields. This is
    // the actual phase1 producer, not a later fog/render publication or a
    // claim that this native display allocation is guest global442F70.
    bool readNativeColdFog273C30(const g::OriginalApplicationEntities50LA& application,
        const ra_cold_sources50LA::Owner& source,std::uint64_t generation,
        g::OriginalRaceFogFields*& out)const noexcept {
        if(terminal_||closed_||cameraPrepareTerminal_||nativeFifoPins_||triggerQueryActive_||
           sharedOwnerLease_||sharedRoleViews_||sharedEffectsStopped_||
           (endCandidateProducer_&&endCandidateProducer_->inCall)||!cameraBankCurrent()||
           !camera_||!camera_->session||!camera_->display||!camera_->fog.current()||
           camera_->fog.source_.get()!=&source||camera_->generation!=generation||
           camera_->fogAddress!=&camera_->fog.fields_||
           camera_->display->trackFog3EC!=&camera_->fog.fields_||nativeStaticDrawPrepared_)return false;
        const auto& s=*camera_->session;
        if(!generation||s.generation_!=generation||s.closed_||s.abandoned_||s.coldBound_||
           s.coldPhase_!=1u||!s.coldViewport_.complete()||!s.coldCameraSelected26F940_||
           s.frameStage_!=original_camera_session50KX::Session::FrameStage::idle||
           s.publicationCount_||s.fogPreparationRevision_||s.fogPreparation_.owner||
           s.displayInputs_!=camera_->display||s.display_.actualTrackFog3EC!=&camera_->fog.fields_||
           s.display_.rendererCurrentScene0!=&s.viewport_||
           s.actual_.applicationEntities.get()!=&application||application.attachmentLock423F58())return false;
        const auto& f=camera_->fog.fields_;std::uint32_t nearBits=0;
        std::memcpy(&nearBits,&f.near54,sizeof(nearBits));
        if(f.far50!=4000.f||nearBits!=0x44a6a99au||f.minimum58!=64.f||f.dirty60!=1u||
           s.display_.rendererFogColor28!=f.color30)return false;
        out=&camera_->fog.fields_;return true;
    }
    std::shared_ptr<original_main_entry50KG::RaceDisplayLifetime50LA>
    nativeColdModelDisplay274160(const g::OriginalApplicationEntities50LA& application,
        const ra_cold_sources50LA::Owner& source,std::uint64_t generation)const noexcept {
        g::OriginalRaceFogFields* fog=nullptr;
        if(!readNativeColdFog273C30(application,source,generation,fog))return {};
        //Actual selected phase1 owner, not a later frame or fabricated display.
        return camera_->session->selectedGameplayDisplay_.lock();
    }
    bool publishNativeColdFog13E99C(const g::OriginalApplicationEntities50LA& application,
        const ra_cold_sources50LA::Owner& source,std::uint64_t generation,
        const g::OriginalRaceFogFields& actual)noexcept {
        g::OriginalRaceFogFields* current=nullptr;
        if(!readNativeColdFog273C30(application,source,generation,current)||current!=&actual)return false;
        // Same real sink already written by coldViewport's fog restoration;
        // later restoration continues to use it. No retained renderer pointer,
        // copied fog state, source setter replay or 442F70 allocation assertion.
        camera_->session->display_.rendererFogColor28=actual.color30;
        return true;
    }
    bool canWithdrawNativeFogBorrow()const noexcept {
        // Fog metadata has no enabled callback/selection route. Its only live
        // native value borrow is withdrawn before closeCamera can destroy it.
        return !nativeFifoPins_&&!triggerQueryActive_&&!sharedOwnerLease_&&!sharedRoleViews_&&
            !(endCandidateProducer_&&endCandidateProducer_->inCall)&&
            (!camera_||cameraBankCurrent());
    }
    bool readNativeLocalFlareCamera(
        const g::original_gc_gameplay_lighting_candidate::NativeRaStaticDraw50LA& record,
        const g::OriginalApplicationEntities50LA& application,g::CameraViewOutput49F& out)const noexcept {
        const auto* publication=nativeStaticDraw();if(!publication)return false;
        bool owned=false;for(const auto& row:*publication)if(&row==&record){owned=true;break;}
        if(!owned||record.kind!=g::original_gc_gameplay_lighting_candidate::NativeRaStaticDraw50LA::Kind::lightFlare||
           !record.flareIdentity||!camera_||!camera_->session)return false;
        const auto& session=*camera_->session;
        if(session.actual_.applicationEntities.get()!=&application||
           session.display_.rendererCurrentScene0!=&session.viewport_||
           &session.actual_.visibility!=freshLighting_.get()||!session.spatial_.registration34.attached||
           !session.spatial_.parent38||session.spatial_.parent38>session.visibilityKeys_.count||
           record.viewport!=std::array<std::int32_t,4>{{session.viewport_.x1E0,session.viewport_.y1E4,
               session.viewport_.width1E8,session.viewport_.height1EC}})return false;
        out=session.matrices_;return true;
    }
    bool prepareNativeStaticDraw(Diagnostic& d)noexcept {
        d={};nativeStaticDrawPrepared_=false;
        StaticFogPublication id;ra_static_fog50LA::CameraObservation observation;
        if(!readStaticFogPublication(id,observation)){
            d.message="current actual RA camera/fog required for native static selection";return false;}
        auto& session=*camera_->session;
        if(!session.actual_.applicationEntities||!session.actual_.applicationMaterials||
           session.display_.rendererCurrentScene0!=&session.viewport_||
           &session.actual_.visibility!=freshLighting_.get()||!session.spatial_.registration34.attached||
           !session.spatial_.parent38||session.spatial_.parent38>session.visibilityKeys_.count){
            d.message="actual RA camera registered parent and viewport required";return false;}
        g::original_gc_gameplay_lighting_candidate::CameraVisibilityLease lease;
        g::original_gc_gameplay_lighting_candidate::Diagnostic detail;
        if(!freshLighting_->beginCameraVisibility(0,lease,detail)||
           !session.visibilityKeys_.matches(*freshLighting_,lease,session.generation_,session.key(session.scene_))){
            d.message="actual RA static selection camera lease unavailable";return false;}
        if(!freshLighting_->prepareNativeRaStaticDraw(lease,*session.actual_.applicationEntities,
            observation.camera,session.spatial_.parent38,session.viewport_,nativeStaticDraw_,detail)){
            d.camera.sourcePC=detail.sourcePC;d.message="native RA portal/model/LOD selection failed";return false;}
        // End the real visibility borrow before validating the idle Bank /
        // camera publication again. This is a value snapshot, not selected18.
        freshLighting_->endCameraVisibility(lease);
        nativeStaticDrawIdentity_=id;nativeStaticDrawPrepared_=true;
        if(!nativeStaticDraw()){nativeStaticDrawPrepared_=false;
            d.message="RA camera/fog changed during native static selection";return false;}
        return true;
    }
    // Inactive partial callback implementation, deliberately not exposed on
    // the aggregate or invoked by pose/fog publication. The eventual real
    //114D00 dispatch must call this from the retained application traversal.
    // Incomplete concrete callbacks stop at their actual source boundary;
    // no ready token is issued. The existing typed camera lease is the full mutation lifetime.
    bool prepareZonePrefix114D00(const g::original_gc_gameplay_lighting_candidate::RaRacerSelectionDispatch50LA& dispatch,Diagnostic& d)noexcept {
        d={};StaticFogPublication id;ra_static_fog50LA::CameraObservation observation;
        if(!readStaticFogPublication(id,observation)){
            d.message="current retained camera/fog required for zone prefix";return false;
        }
        auto& session=*camera_->session;
        if(!session.actual_.applicationEntities||!session.actual_.applicationMaterials||
           session.display_.rendererCurrentScene0!=&session.viewport_||
           &session.actual_.visibility!=freshLighting_.get()||
           !session.spatial_.registration34.attached||!session.spatial_.parent38||
           session.spatial_.parent38>session.visibilityKeys_.count){
            d.message="actual renderer-selected camera and registered parent38 required";return false;
        }
        g::original_gc_gameplay_lighting_candidate::CameraVisibilityLease lease;
        g::original_gc_gameplay_lighting_candidate::Diagnostic detail;
        if(!freshLighting_->beginCameraVisibility(0,lease,detail)||
           !session.visibilityKeys_.matches(*freshLighting_,lease,session.generation_,session.key(session.scene_))){
            d.message="actual camera/world lease unavailable";return false;
        }
        g::OriginalFollowSceneBinding actual;
        if(!g::readOriginalFollowSceneBinding(session.spatial_,session.visibilityKeys_.view(),actual)||
           actual.sceneKey!=session.sceneBinding_.sceneKey||
           actual.parentZoneKey!=session.sceneBinding_.parentZoneKey||
           actual.registrationKey!=session.sceneBinding_.registrationKey){
            d.message="actual camera registration changed";return false;
        }
        const bool completed=freshLighting_->prepareRaZonePrefix114D00(lease,
            *session.actual_.applicationEntities,session.matrices_,session.spatial_.parent38,session.viewport_,
            *session.actual_.applicationMaterials,session.racerEntities_,session.spatial_,dispatch,session.displayInputs_->rendererDispatch,
            session.displayInputs_->renderer,session.displayInputs_->diagnostic,detail);
        d.camera.sourcePC=detail.sourcePC;
        d.message="partial selection prefix only; inspect sourcePC for the remaining source boundary";
        return completed;
    }
    // Unissued: the actual application phase5/camera114D50 prefix must own
    // this entry, including prior selection/camera-child completion and the
    // SAME still-active visibility lease. A pose/fog publication cannot make it.
    class ZoneDrawEntry114E00 final {
        friend class OwnershipBridge;
        OwnershipBridge* owner_;
        g::original_gc_gameplay_lighting_candidate::CameraVisibilityLease* visibility_;
        std::shared_ptr<::original_main_entry50KG::NativeCollectorFifo50LA> collector_;
        ZoneDrawEntry114E00(OwnershipBridge& owner,
            g::original_gc_gameplay_lighting_candidate::CameraVisibilityLease& visibility,
            std::shared_ptr<::original_main_entry50KG::NativeCollectorFifo50LA> collector)
            :owner_(&owner),visibility_(&visibility),collector_(std::move(collector)){}
    };
    bool drawZonePrefix114E00(const ZoneDrawEntry114E00&,Diagnostic&)noexcept;
    bool prepareFog(Diagnostic& d)noexcept {
        d={};StaticLightPublication base;
        if(!readStaticLightPublication(base)||!camera_->fog.current()||!camera_->display||
           camera_->display->trackFog3EC!=&camera_->fog.fields_||
           camera_->session->displayInputs_!=camera_->display){
            d.message="actual published RA SourceCamera and retained fog required";return false;
        }
        d.camera=camera_->session->prepareGameplayFogRender();
        if(d.camera.status!=original_camera_session50KX::Status::complete){
            d.message="actual RA fog camera preparation failed";return false;
        }
        StaticFogPublication id;ra_static_fog50LA::CameraObservation observation;
        return readStaticFogPublication(id,observation);
    }
    std::shared_ptr<const ra_static_candidates50LA::Owner> candidates_;
    RuntimeQueryCapacityInputs inputs_;
    std::size_t recoveryMeshStack_=0,wreckMeshStack_=0;
    storage::Limits limits_{};
    storage::Owner storage_;
    std::array<std::unique_ptr<storage::Membership>,2> triggerMembership_;
    // Shared across all racer banks: never snapshot action/contact history.
    ra_trigger_actions50LA::ActionLease triggerLease_{};
    bool triggerQueryActive_=false;
    std::unique_ptr<ActualQueryOwners> actual_;
    //Only the same-Application private cold roster producer binds these.
    //Aliasing shared ownership pins the existing process context, not a copy
    //or a Bank lane. It survives every serial racer visit and partial stop.
    std::shared_ptr<g::CoupledNoiseContext49M> sharedSecondaryNoiseOwner_;
    std::weak_ptr<g::OriginalApplicationEntities50LA> sharedSecondaryNoiseEntities_;
    std::unique_ptr<original_follow_query_bridge::QueryBridge> follow_;
    struct CameraBinding final {
        // Factory-owned lifetime pins for the existing persistent collection.
        // No second Cables object or source material bank is created.
        std::array<std::unique_ptr<native_racer50HW::Cables::SourceCameraPin>,8> cablePins;
        std::array<native_racer50HW::Cables*,8> cables{};
        std::shared_ptr<g::original_gc_gameplay_lighting_candidate::Owner> lighting;
        std::shared_ptr<g::OriginalTrackParticleCold50LA> particles;
        std::vector<std::uint8_t> cam,tob,phySnapshot;
        const native_racer50HW::Rig* selectedRig=nullptr;
        original_camera_session50KX::AuthoredInputs authored{};
        original_camera_session50KX::DisplayInputs* display=nullptr;
        std::uint64_t generation=0;
        ra_track_fog50LA::Owner fog;
        rr::game::OriginalRaceFogFields* fogAddress=nullptr;
        // Last: real Session dies before every owned authoring/manager borrow.
        std::unique_ptr<original_camera_session50KX::Session> session;
        ~CameraBinding(){
            if(session&&!session->closed_)std::terminate();
            session.reset(); // Destroy the closed Session before retiring its fog borrow.
            if(display&&fogAddress&&display->trackFog3EC==fogAddress)display->trackFog3EC=nullptr;
        }
    };
    std::unique_ptr<CameraBinding> camera_;
    // UNACTIVATED: the actual issuer selects its OWN retained Session and
    // generation. No caller Session/key/generation or public grant is accepted.
    bool constructCurrentColdController174A30()noexcept {
        if(terminal_||closed_||cameraPrepareTerminal_||!camera_||!camera_->session||
           !camera_->generation||!camera_->display||!cameraBankCurrent()||
           !freshLighting_||camera_->lighting!=freshLighting_||!camera_->fog.current()||
           camera_->display->trackFog3EC!=&camera_->fog.fields_)return false;
        return camera_->session->constructColdController174A30(camera_->generation);
    }
    bool constructCurrentRaIntro174FA4(const original_main_entry50KG::Application&)noexcept;
    rr::game::OriginalWreckComponent50LA* currentIntroController174A30()noexcept {
        if(terminal_||closed_||cameraPrepareTerminal_||!camera_||!camera_->session||!cameraBankCurrent()||
           camera_->session->closed_||!camera_->session->nativeTaIssuance_||
           camera_->session->nativeTaIssuance_->course!=g::OriginalCameraStartCourse::ra)return nullptr;
        return camera_->session->taControllerEntity174A30();
    }
    bool constructCurrentIntroAudio177750(std::shared_ptr<const rr::assets::AudioBank50HC> bank,
        std::size_t firstVoice,std::size_t voiceCount)noexcept {
        return currentIntroController174A30()&&camera_->session->constructTaIntro177750(std::move(bank),firstVoice,voiceCount);
    }
    bool cameraAttempted_=false,cameraPrepareTerminal_=false;
    bool storageAttempted_=false,capacitySealed_=false,storageConstructed_=false;
    bool bindAttempted_=false,queriesConstructed_=false,terminal_=false,closed_=false;
    bool nativeWreckAttempted_=false,nativeWreckPrepared_=false,nativeWreckRetired_=false;
    bool sourceIncomingZonesAttempted_=false,sourceIncomingZonesReady_=false,sourceIncomingZonesWithdrawing_=false,sourceIncomingZonesWithdrawn_=false;
    const void* sourceIncomingVisibility_=nullptr;
    const void* sourceIncomingCollision_=nullptr;
    g::ResetVisibilityWorld50EY* sourceIncomingVisibilityWorld_=nullptr;
    g::LiveZoneWorld50EN* sourceIncomingCollisionWorld_=nullptr;
    g::ZoneLifecycle50CL* sourceIncomingCollisionLifecycle_=nullptr;
    std::uint32_t sourceIncomingVisibilityOrdinal_=0,sourceIncomingCollisionOrdinal_=0;
    bool bindFreshSourceIncomingZones(original_camera_session50KX::Session&)noexcept;
    bool withdrawFreshSourceIncomingZones(original_camera_session50KX::Session&)noexcept;
    bool retireFreshSourceIncomingZones(original_camera_session50KX::Session&)noexcept;
    bool loadFreshSourceWreckSecondBody165E90(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool loadFreshSourceWreckBody165E90(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool loadFreshSourceWreckSecondE0166018(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool loadFreshSourceWreckE0166018(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool loadFreshSourceWreckSecondE0Shape166074(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool loadFreshSourceWreckE0Shape166074(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool prepareAppendFreshSourceWreckSecondE016607C(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool prepareAppendFreshSourceWreckE016607C(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool constructFreshSourceWreckSecondDC1660A0(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool constructFreshSourceWreckDC1660A0(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool constructFreshSourceWreckSecondD816616C(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool constructFreshSourceWreckD816616C(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool constructLoadFreshSourceWreckSecondE41661D8(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool constructLoadFreshSourceWreckE41661D8(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool prepareAppendFreshSourceWreckSecondE416629C(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool prepareAppendFreshSourceWreckE416629C(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool bindFreshSourceWreckSecondEffectInputs2CB618(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool constructFreshSourceWreckSecondMutual166390(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool constructFreshSourceWreckSecondE81662E4(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool constructFreshSourceWreckE81662E4(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool stopFreshSourceWreckSecondFirstPair2CB778(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool stopFreshSourceWreckFirstPair2CB778(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool loadFreshSourceWreckSecondPiece22C0A0(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool advanceFreshSourceWreckSecondPieceCursor22C25C(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool bindFreshSourceWreckGroupPrimary22C298(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool consumeFreshSourceWreckGroupContext2CC088(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool constructFreshSourceWreckGroupSparks2CC0B4(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool setupFreshSourceWreckGroupSparks2CC0D4(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool beginFreshSourceWreckGroupSparksConfig15F2F0(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool constructFreshSourceWreckGroup22C278(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool advanceFreshSourceWreckPieceCursor22C25C(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool applyFreshSourceWreckSecondWater22C21C(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool applyFreshSourceWreckWater22C21C(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool appendFreshSourceWreckSecondPiece22C1B8(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool appendFreshSourceWreckPiece22C1B8(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool readStopAppendFreshSourceWreckSplash19DDC8(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool readStopAppendFreshSourceWreckDust19A7D0(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool readAppendFreshSourceWreckFire15E024(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool completeFreshSourceWreckSecondInitialChild19A7F8(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool completeFreshSourceWreckSecondSuccessorChild(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool readFreshSourceWreckSecondSuccessorChild(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool readStopAppendFreshSourceWreckSecondPositiveDust19A7D0(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool readStopAppendFreshSourceWreckSecondPositiveSplash19DDC8(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool readFreshSourceWreckSecondInitialChild19A7D0(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool readAppendFreshSourceWreckSmoke19A7D0(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool continueFreshSourceWreckSecondPairs2CB6CC(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool continueFreshSourceWreckPairs2CB6CC(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool constructFreshSourceWreckFire2CB720(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool constructFreshSourceWreckSplash2CB854(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool constructFreshSourceWreckSecondPositiveSplash(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool constructFreshSourceWreckSecondSuccessorChild(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool constructFreshSourceWreckSecondPositiveDust(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool constructFreshSourceWreckSecondInitialChild(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool constructFreshSourceWreckDust2CB7E0(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool constructFreshSourceWreckSmoke2CB6CC(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool constructFreshSourceWreckSecondArrays2CB628(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool constructFreshSourceWreckArrays2CB628(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool constructFreshSourceWreckMutual166390(std::size_t,g::OriginalRacerEntityCold50LA&,std::uint32_t&)noexcept;
    bool sourceWreckConstructorAttempted_=false,sourceWreckConstructorWithdrawing_=false,sourceWreckConstructorRetired_=false;
    bool loadFreshSourceWreckPiece212A50(std::size_t,g::OriginalRacerEntityCold50LA&,
        std::uint32_t&)noexcept;
    bool loadFreshSourceWreckConfig2129A4(std::size_t,g::OriginalRacerEntityCold50LA&,
        const std::vector<std::uint8_t>&,std::uint32_t&)noexcept;
    bool retireFreshSourceWreckConstructors(original_camera_session50KX::Session&)noexcept;

    bool stageNativeRetainedWreckCold(original_camera_session50KX::Session&)noexcept;
    ::ra_pod_scene50LA::Owner* nativeWreckZoneOwner_=nullptr;
    g::OriginalTrackParticleCold50LA* nativeWreckParticles_=nullptr;
    bool nativeFirstWreckApplicationCurrent(const original_camera_session50KX::Session&,
        const original_main_entry50KG::Application&)const noexcept;
    bool nativeRacerInputOwnerCurrent(original_camera_session50KX::Session&,
        const native_racer_model50LA::Owner&)const noexcept;
    bool nativeHandlingInputOwnerCurrent(original_camera_session50KX::Session&,
        const native_racer_model50LA::Owner&)const noexcept;
    rr::assets::NativePhyLoad50LA* nativePhyOwnerCurrent(original_camera_session50KX::Session&,
        const native_racer_model50LA::Owner&)const noexcept;
    bool readNativeHandlingTrack187330(original_camera_session50KX::Session&,
        const native_racer_model50LA::Owner&,const g::OriginalRaceConstructor50LA&,std::array<char,3>&)const noexcept;
    bool readNativeHandlingConfiguration215434(original_camera_session50KX::Session&,
        const native_racer_model50LA::Owner&,std::uint32_t&)const noexcept;
    ra_cold_roster50LA::Operation* nativeRacerOperationCurrent(original_camera_session50KX::Session&,
        const native_racer_model50LA::Owner&)const noexcept;
    ra_cold_roster50LA::Operation* nativeHandlingOperationCurrent(original_camera_session50KX::Session&,
        const native_racer_model50LA::Owner&)const noexcept;
    bool readNativeHandlingContact215900(original_camera_session50KX::Session&,
        const native_racer_model50LA::Owner&,g::SpringGeneratedParameters48K*&,
        g::SpringResponseParameters48L*&)const noexcept;
    bool publishNativeHandlingContact215A70(original_camera_session50KX::Session&,
        const native_racer_model50LA::Owner&)const noexcept;
    bool readNativeHandlingCooling215CC8(original_camera_session50KX::Session&,
        const native_racer_model50LA::Owner&,g::BoostParameters50FK*&,float*&)const noexcept;
    bool readNativeHandlingProfile216040(original_camera_session50KX::Session&,
        const native_racer_model50LA::Owner&,const g::OriginalRaceConstructor50LA&,
        bool&,std::array<float,6>&)const noexcept;
    bool publishNativeHandlingDifference21600C(original_camera_session50KX::Session&,
        const native_racer_model50LA::Owner&,float)const noexcept;
    bool bindNativePlayerSpeechForward22A210(ra_cold_roster50LA::Operation&)noexcept;
    bool withdrawNativePlayerSpeechForward22A210(ra_cold_roster50LA::Operation&)noexcept;
    bool continueNativeBaseRacerSpeech22A1D0(original_camera_session50KX::Session&,
        native_racer_model50LA::Owner&,const g::OriginalRaceConstructor50LA&,std::uint32_t&)noexcept;
    bool retireNativePlayerSpeechCold(original_camera_session50KX::Session&)noexcept;
    ra_cold_roster50LA::Operation* nativePlayerOperationCurrent(original_camera_session50KX::Session&,
        const native_racer_model50LA::Owner&)const noexcept;
    bool bindNativePlayerLoad1A0848(original_camera_session50KX::Session&,
        native_racer_model50LA::Owner&)noexcept;
    bool continueNativePlayerReturn1A0850(original_camera_session50KX::Session&,
        native_racer_model50LA::Owner&,std::uint32_t&)noexcept;
    bool continueNativePlayerResources1AE104(original_camera_session50KX::Session&,
        native_racer_model50LA::Owner&,const g::OriginalRaceConstructor50LA&,std::uint32_t&)noexcept;
    bool continueNativeOuterStorage212E18(original_camera_session50KX::Session&,
        native_racer_model50LA::Owner&,std::uint32_t&)noexcept;
    // PodLoad (original_pod_load50pl.cpp): 2129C0 for every slot in 1AE280
    // manager order, track-side inputs only through TrackDataProvider50PL.
    friend class ::original_pod_load50PL::SceneTrackData50PL;
    bool loadPodRosterFromScene2129C0(original_camera_session50KX::Session&,const g::OriginalRaceConstructor50LA&,
        ::ra_pod_scene50LA::Owner&,const void* water3FC,const g::UndertrackPhysicsDomain50EW&,const std::string&,
        const std::string&,std::uint32_t&,std::uint32_t&)noexcept;
    bool loadPodRoster2129C0(original_camera_session50KX::Session&,const g::OriginalRaceConstructor50LA&,
        ::original_pod_load50PL::TrackDataProvider50PL&,const g::UndertrackPhysicsDomain50EW&,const std::string&,
        const std::string&,std::uint32_t&,std::uint32_t&)noexcept;
    bool loadPodRacer2129C0(std::size_t slot,original_camera_session50KX::Session&,const g::OriginalRaceConstructor50LA&,
        const g::OriginalWreckSetupZones50LA&,const g::UndertrackPhysicsDomain50EW&,const std::string&,
        const std::string&,const void*,std::uint32_t&,std::uint32_t&)noexcept;
    bool podSceneRetainZones(::ra_pod_scene50LA::Owner&,std::uint64_t,g::OriginalWreckSetupZones50LA&,
        g::CollectionRef50BX&,g::CollectionRef50BX&)noexcept;
    bool podSceneReleaseZones(::ra_pod_scene50LA::Owner&)noexcept;
    bool podSceneRetainParticles(::ra_pod_scene50LA::Owner&,g::OriginalTrackParticleCold50LA*)noexcept;
    bool podWarmCreators(original_camera_session50KX::Session&,std::size_t slot,
        std::array<g::OriginalWreckWarmCreator50LA,4>&,std::size_t&)const noexcept;
    // Roster-wide 4C0[0]/4C4[0] borrow (1AE4B0) and the zone fields 165E90 reads.
    bool podRosterAttempted_=false,podZonesHeld_=false;
    ::original_pod_load50PL::TrackDataProvider50PL* podTrack_=nullptr;
    std::unique_ptr<::original_pod_load50PL::TrackDataProvider50PL> podTrackStorage_; //Provider lifetime spans the zone borrow.
    g::OriginalWreckSetupZones50LA podZones_{};
    g::CollectionRef50BX podVisibility_=0,podCollision_=0;
    std::array<bool,8> podAiHandlingStopped_{}; //STAND-IN: AI 16E5F4 handling not written.
    ::original_pod_load50PL::RacerExclusionLanes50PL podExclusions_; //212ABC per-racer 690 lanes (one layout, pod_load50pl.h).

    bool retireNativeRetainedWreckCold(original_camera_session50KX::Session&)noexcept;

    std::uint32_t nativeFifoPins_=0; // Queued actual camera/allocation borrow, not source state.
    bool registrationAttempted_=false,worldOwnersRegistered_=false;
    std::unique_ptr<GridArenaOwners> gridArena_;
    std::vector<std::uint32_t> groundStack_;
    std::array<bool,8> gridAttempted_{};
    struct GridCompletion {
        bool complete=false;
        g::BodyMatrix48G main{},physical{};
        const g::Type2ResourceView49X* mainResource=nullptr;
        const g::Type2ResourceView49X* secondaryResource=nullptr;
        std::uint64_t mainRevision=0,secondaryRevision=0,terrainRevision=0;
    };
    std::array<GridCompletion,8> gridCompletion_{};
    std::shared_ptr<g::original_gc_gameplay_lighting_candidate::Owner> freshLighting_;
    std::unique_ptr<FreshRecoveryArenaOwners> freshRecovery_;
    std::array<bool,8> freshAttempted_{},freshComplete_{},freshRetirementRequired_{};
    // The context is one actual arena binding. Bank slots retain its pointer,
    // while its referenced shape/member follow the Bank's selected arena.
    std::unique_ptr<const g::PodLiveQueryContext50IZ> podContext_;
    g::VehicleFrameBindings49W* podBindings_=nullptr;
    g::VehicleFrameParameters49W* podParameters_=nullptr;
    std::array<bool,8> podBindingInstalled_{};
    // One immutable role split accompanies the retained shared pod context.
    std::uint32_t podHumanCount_=0;
    // Outside Bank snapshots: real cleanup must discharge every reached slot.
    std::array<bool,8> gridRetirementRequired_{};
    std::array<const native_racer50HW::Rig*,8> gridRigIdentity_{};

    // Weak only: there is no bridge -> Bank/pin -> bridge strong cycle.
    std::weak_ptr<native_racer50HN::Bank::RaWorldLifetime> bankLifetime_;
    const native_racer50HN::Bank* bankIdentity_=nullptr;
    struct BankPin final : native_racer50HN::Bank::RaWorldLifetime {
        std::shared_ptr<OwnershipBridge> owner;
        // Sole lifetime pin for partial model/visual prefixes. Operation has
        // no strong Bank/pin link, so this introduces no ownership cycle.
        std::shared_ptr<ra_cold_roster50LA::Operation> rosterOperation;
        BankPin(native_racer50HN::Bank& bank,std::shared_ptr<OwnershipBridge> actual)noexcept
            :RaWorldLifetime(bank),owner(std::move(actual)){}
        bool canRetire()const noexcept override {return owner&&owner->canRetireBankBacking();}
        ~BankPin() override {
            // Bank's slots/spans have already died. Do not touch Bank here.
            // SourceCamera must have performed its real bank0 close first.
            owner->verifyFollowRetiredForBankDestruction();
        }
    };
    bool canRetireBankBacking()const noexcept {
        if(sharedOwnerLease_||sharedRoleViews_||sharedEffectsStopped_)return false;
        if(nativeFifoPins_||triggerQueryActive_||rosterRetirementRequired_)return false;
        for(const bool pending:freshRetirementRequired_)if(pending)return false;
        for(const bool pending:gridRetirementRequired_)if(pending)return false;
        return (!camera_||!camera_->session||camera_->session->closed_)&&
            (!follow_||!follow_->attempted_||follow_->closed_);
    }
    void verifyFollowRetiredForBankDestruction()const noexcept {
        if(!canRetireBankBacking())std::terminate();
    }
    explicit OwnershipBridge()=default;
    bool fail(Diagnostic& d,const char* why)noexcept {
        terminal_=true;for(auto& record:gridCompletion_)record.complete=false;
        freshComplete_.fill(false);d.message=why;return false;
    }
    static bool meshExtent(const rr::assets::CollisionMesh48I& mesh,std::size_t& value)noexcept {
        const auto maximum=std::numeric_limits<std::size_t>::max();
        if(mesh.faces.size()==maximum||mesh.nodes.size()>maximum-mesh.faces.size()-1u)return false;
        value=mesh.nodes.size()+mesh.faces.size()+1u;return true;
    }
    bool plannedIdentityFree(std::uint32_t id)const noexcept {
        if(!id||id==native_query_identities50KX::followSphere24||
           id==native_query_identities50KX::followRay190||id==native_query_identities50KX::death120)return false;
        const auto* bindings=candidates_->staticBindings();
        for(const auto& b:*bindings)if(b.candidate->candidate.collision.filter.identity==id)return false;
        return true;
    }
    // Private fresh-only alternate factory. No existing loader calls this.
    // Plan points to the actual retained roster during synchronous allocation;
    // Storage copies typed owner borrows, never a saved numeric World image.
    bool constructSharedRosterStorage(std::shared_ptr<g::OriginalApplicationEntities50LA> app,
        std::shared_ptr<const ra_static_candidates50LA::Owner> candidates,
        const RuntimeQueryCapacityInputs& input,Diagnostic& d)noexcept {
        if(!rosterResources_||!input.recovery||bankIdentity_||!bankLifetime_.expired())
            return fail(d,"shared roster requires fresh retained owners before Bank registration");
        const storage::Owner::SharedRosterPlan plan{input.collisionRoster.data(),input.collisionRoster.size()};
        return constructStorage(std::move(app),std::move(candidates),input,d,&plan);
    }
    bool constructStorage(std::shared_ptr<g::OriginalApplicationEntities50LA> app,
        std::shared_ptr<const ra_static_candidates50LA::Owner> candidates,
        const RuntimeQueryCapacityInputs& input,Diagnostic& d,
        const storage::Owner::SharedRosterPlan* shared=nullptr)noexcept {
        d={};
        if(storageAttempted_||terminal_||closed_||!app||!app->application10())
            return fail(d,"RA runtime storage requires fresh state and actual Application");
        storageAttempted_=true;
        try {
            const auto* source=candidates?candidates->decodedSource():nullptr;
            const auto* bindings=candidates?candidates->staticBindings():nullptr;
            if(!source||!bindings||source->collisions.size()!=178u||bindings->size()!=(candidates->triggerActions_?180u:178u)||
               input.humanCount>1u||input.humanCount>input.collisionRoster.size()||input.collisionRoster.empty()||input.collisionRoster.size()>8u||
               input.wreck.size()>(storage::runtimeSearchSlotLimit-8u)/3u)
                return fail(d,"actual RA static source and bounded original collision roster required");
            for(std::size_t i=0;i<bindings->size();++i){const auto& b=(*bindings)[i];
                if(!b.candidate||b.animated||b.trigger!=(i>=178u)||
                   (i>=178u&&(!b.candidate->raOriginalTriggerSegments||b.authoredZone!=4u||
                    b.candidate->nativeTriggers!=&candidates->triggerActions_->nativeGroup()||
                    b.candidate->nativeTriggerIndex!=i-178u)))
                    return fail(d,"RA requires original static prefix then its two authored trigger owners");
            }
            if(input.recovery&&(input.recovery->storagePrepared||input.recovery->sourcePrepared))
                return fail(d,"recovery reservation requires its fresh actual owner before world-sized allocation");
            candidates_=std::move(candidates);
            // Copy only typed borrows/planned native identities. Caller containers
            // may die; their actual owners/resources must remain alive and stable.
            inputs_=input;
            for(std::size_t i=0;i<inputs_.collisionRoster.size();++i){
                const auto* owner=inputs_.collisionRoster[i];
                if(!owner||!owner->prepared())return fail(d,"unprepared original roster collision owner");
                for(std::size_t j=0;j<i;++j)if(inputs_.collisionRoster[j]==owner)
                    return fail(d,"duplicate original roster collision owner");
                std::size_t extent=0;
                if(!meshExtent(owner->mainTarget().mesh,extent))return fail(d,"recovery mesh extent overflow");
                if(inputs_.recovery)recoveryMeshStack_=std::max(recoveryMeshStack_,extent);
            }
            if(rosterResources_)for(auto id:rosterAuxiliaryIds_)
                if(!plannedIdentityFree(id))return fail(d,"actual auxiliary namespace conflicts with RA source/camera");
            // Wreck prepareCold needs the allocated query world, so only genuine
            // unprepared objects and prepared immutable assets may reserve here.
            for(std::size_t i=0;i<inputs_.wreck.size();++i){
                const auto& w=inputs_.wreck[i];
                if(!w.queries||!w.assets||!w.assets->prepared()||w.queries->prepared()||w.queries->constructed())
                    return fail(d,"wreck reservation requires fresh real queries and prepared assets");
                std::size_t extent=0;
                if(!meshExtent(w.assets->targetE4().mesh,extent))return fail(d,"wreck mesh extent overflow");
                wreckMeshStack_=std::max(wreckMeshStack_,extent);
                for(std::size_t j=0;j<i;++j)if(inputs_.wreck[j].queries==w.queries||inputs_.wreck[j].assets==w.assets)
                    return fail(d,"duplicate actual wreck query or asset owner");
                for(unsigned q=0;q<3;++q){
                    for(auto id:rosterAuxiliaryIds_)if(id&&id==w.identities[q])
                        return fail(d,"planned wreck identity conflicts with actual auxiliary owner");
                    if(!plannedIdentityFree(w.identities[q])||
                       w.identities[q]==native_query_identities50KX::movement7A4||
                       w.identities[q]==native_query_identities50KX::secondary364||
                       w.identities[q]==native_query_identities50KX::terrain348||
                       w.identities[q]==native_query_identities50KX::pod694)return fail(d,"wreck planned identity conflicts with actual source namespace");
                    for(std::size_t j=0;j<=i;++j)for(unsigned k=0;k<(j==i?q:3u);++k)
                        if(inputs_.wreck[j].identities[k]==w.identities[q])return fail(d,"duplicate planned wreck identity");
                }
            }
            limits_.originalCameraQueries=true;
            limits_.originalRecoveryQueries=inputs_.recovery!=nullptr;
            limits_.wreckPieceQueries=inputs_.wreck.size();
            const std::size_t base=shared?3u+inputs_.collisionRoster.size()*storage::Storage::rosterRoles:
                6u+(inputs_.recovery?2u:0u);
            if(inputs_.wreck.size()>(storage::runtimeSearchSlotLimit-base)/3u||
               bindings->size()>storage::runtimeSearchSlotLimit-base-inputs_.wreck.size()*3u)
                return fail(d,"actual query reservation exceeds one-block registry capacity");
            // Capacity is fixed before lease acquisition or any Storage effects.
            // It cannot be widened or swapped beneath a surviving registry.
            // Bounded HOST identities, not authored pointers or collection refs.
            // Reserve before any Storage mutation; the immutable association is
            // the existing collisionRoster order. Actual arena IDs are assigned
            // only at the per-slot source resource-assignment phase.
            std::uint32_t next=1;
            auto available=[&](std::uint32_t id){
                if(!plannedIdentityFree(id))return false;
                constexpr std::uint32_t auxiliary[]={native_query_identities50KX::movement7A4,
                    native_query_identities50KX::secondary364,native_query_identities50KX::terrain348,
                    native_query_identities50KX::pod694};
                for(auto value:auxiliary)if(id==value)return false;
                for(auto value:rosterAuxiliaryIds_)if(id==value)return false;
                if(inputs_.recovery)for(auto value:inputs_.recovery->globals.identity)if(id==value)return false;
                for(const auto& w:inputs_.wreck)for(auto value:w.identities)if(id==value)return false;
                return true;
            };
            for(std::size_t slot=0;slot<inputs_.collisionRoster.size();++slot){
                auto& row=bodyIdentities_[slot];row.collision=inputs_.collisionRoster[slot];
                std::uint32_t* targets[]={&row.main,&row.secondary,&row.terrain,&row.pod,&row.movement};
                const auto targetCount=shared?5u:2u;
                for(unsigned targetIndex=0;targetIndex<targetCount;++targetIndex){
                    auto* target=targets[targetIndex];
                    while(next&&!available(next)){
                        if(next==std::numeric_limits<std::uint32_t>::max())next=0;else ++next;
                    }
                    if(!next)return fail(d,"RA native body identity namespace exhausted");
                    *target=next;
                    if(next==std::numeric_limits<std::uint32_t>::max())next=0;else ++next;
                }
            }
            if(candidates_->triggerActions_){
                if(inputs_.collisionRoster.empty()||!rosterResources_)
                    return fail(d,"RA triggers require the actual active retained roster");
                std::array<ra_trigger_actions50LA::RacerIdentities,8> ids{};
                for(std::size_t i=0;i<inputs_.collisionRoster.size();++i)ids[i]={bodyIdentities_[i].main,bodyIdentities_[i].secondary};
                // Preserve the actual source PLAYER prefix for IgnorePlayer.
                // Normal Course owns Player0; legacy ALL8 has an empty prefix.
                if(!candidates_->triggerActions_->prepare(*candidates_->triggerSource_,ids,inputs_.humanCount,source->ai.track.count,inputs_.collisionRoster.size()))
                    return fail(d,"RA original trigger identity/action preparation failed");
            }
            capacitySealed_=true;
            auto lease=ra_cold_sources50LA::Owner::leaseLiveWorld(candidates_->source_);
            if(!lease)return fail(d,"same-source RA live world already leased or invalid");
            const storage::SourceWorldView<ra_cold_sources50LA::CollisionOwner> view{
                source->world,{source->collisions.data(),source->collisions.size()}};
            const bool prepared=shared?
                storage_.prepareSharedRosterSource(view,bindings->data(),bindings->size(),limits_,d.storage,
                    lease->world(),lease,std::move(app),*shared):
                storage_.prepareSource(view,bindings->data(),bindings->size(),limits_,d.storage,
                    &lease->world(),lease,std::move(app));
            if(!prepared)return fail(d,"RA runtime static/query storage construction failed");
            storageConstructed_=true;
            if(candidates_->triggerActions_){auto& s=*storage_.get();
                for(std::size_t i=0;i<2;++i){
                    triggerMembership_[i]=std::make_unique<storage::Membership>(s.count.membership);
                    auto& m=*triggerMembership_[i];const auto& authored=(*candidates_->triggerSource_->sources())[i];
                    const auto bounds=ra_trigger_candidates50LA::bounds(authored);
                    if(g::attachLiveZoneMembership50EN(s.lifecycle,s.world,m.state,m.storage(),
                        static_cast<g::CollectionRef50BX>(179u+i),authored.zone,g::ZoneMethod50CL::First,0.f,
                        bounds,s.membershipWork).status!=g::LiveZoneStatus50EN::Complete||
                       g::updateLiveZoneMembership50EN(s.lifecycle,s.world,m.state,bounds,s.membershipWork).status!=
                        g::LiveZoneStatus50EN::Complete)return fail(d,"RA original trigger membership rejected");
                }
            }
            return true;
        }catch(...){return fail(d,"RA runtime storage failure; retained prefix is terminal");}
    }
    bool bindQueries(const ActualQueryOwners& a,Diagnostic& d)noexcept {
        d={};
        // Stage B deliberately lends NO legacy role/Follow/reset views. Their
        // typed shared-owner producers/consumers belong to later reviewed work.
        if(storage_.get()&&storage_.get()->sharedRoster_)
            return fail(d,"shared roster query-owner publication is not implemented");
        return bindQueryArena(a,d,false);
    }
    //Private and uncalled: admits only the actual typed arena, NOT Follow,
    //fixed-role query views, a Bank pin, membership or gameplay readiness.
    bool bindSharedQueryArena(const ActualQueryOwners& a,Diagnostic& d)noexcept {
        d={};
        if(!storage_.get()||!storage_.get()->sharedRoster_)
            return fail(d,"shared arena requires the actual private fresh factory");
        return bindQueryArena(a,d,true);
    }
    bool bindQueryArena(const ActualQueryOwners& a,Diagnostic& d,bool shared)noexcept {
        if(!storage_.get()||shared!=bool(storage_.get()->sharedRoster_))
            return fail(d,"typed arena mode differs from actual fresh allocation");
        if(terminal_||closed_||bindAttempted_||!storageConstructed_||!capacitySealed_)
            return fail(d,"RA actual query binding requires its fresh sealed storage");
        bindAttempted_=true;
        try {
            if(rosterResources_&&(&a.terrain!=rosterTerrain_||a.movement!=rosterMovement_||!a.recovery||
                a.terrain.metadata.filter.identity!=rosterAuxiliaryIds_[0]||
                a.recovery->globals.terrainIdentity348!=rosterAuxiliaryIds_[0]||
                a.recovery->globals.identity[0]!=rosterAuxiliaryIds_[1]||
                a.recovery->globals.identity[1]!=rosterAuxiliaryIds_[2]||
                a.movement->filter.identity!=rosterAuxiliaryIds_[2]))
                return fail(d,"actual auxiliary constructors differ from retained native reservation");
            for(const auto& row:bodyIdentities_)if(row.collision){
                for(auto id:{row.main,row.secondary})
                    if(id==a.terrain.metadata.filter.identity||
                       (a.movement&&id==a.movement->filter.identity))
                        return fail(d,"actual auxiliary identity conflicts with sealed native body reservation");
            }
            auto& s=*storage_.get();
            bool selected=false;for(const auto* item:inputs_.collisionRoster)if(item==&a.selectedCollision)selected=true;
            if(!selected||!a.selectedCollision.prepared()||a.frame.phase!=g::VehicleFramePhase49W::Ready||
               a.main.resource!=&a.selectedCollision.main()||a.secondary.resource!=&a.selectedCollision.secondary()||
               a.main.filter!=&a.mainFilter||a.secondary.filter!=&a.secondaryFilter||
               !a.main.queryScene.value||!a.secondary.queryScene.value||a.main.queryScene.value==a.secondary.queryScene.value||
               !a.main.normalMode54Bound||a.main.normalMode54!=0u||
               !a.secondary.normalMode54Bound||a.secondary.normalMode54!=0u||
               !a.terrain.prepared||
               a.terrain.receipt.archiveBytes!=a.selectedCollision.receipt().archiveBytes||
               a.terrain.receipt.podBoxMBytes!=a.selectedCollision.receipt().memberBytes[0]||
               !a.terrain.metadata.normalMode54Bound||a.terrain.metadata.normalMode54!=1u||
               !live_terrain_probe50fr::matches(a.terrainLive,a.terrain.initial)||
               s.mainMembership.state.attached||s.secondaryMembership.state.attached||s.terrainMembership.state.attached)
                return fail(d,"actual Ready arena, selected resources or terrain/query constructor ownership differs");
            for(const auto& binding:*candidates_->staticBindings())
                if(binding.candidate->candidate.collision.typeTag!=a.main.collisionTypeTag)
                    return fail(d,"actual RA static and query collision type namespaces differ");
            const auto mainId=a.mainFilter.identity,secondaryId=a.secondaryFilter.identity;
            const auto terrainId=a.terrain.metadata.filter.identity;
            if(!mainId||!secondaryId||!terrainId||mainId==secondaryId||mainId==terrainId||secondaryId==terrainId||
               !plannedIdentityFree(mainId)||!plannedIdentityFree(secondaryId)||!plannedIdentityFree(terrainId))
                return fail(d,"actual body/terrain query identities conflict");
            if(a.recovery!=inputs_.recovery||((a.recovery!=nullptr)!=(a.movement!=nullptr)))
                return fail(d,"actual recovery route differs from sealed reservation");
            if(a.recovery){
                const auto& r=*a.recovery;
                if(!r.storagePrepared||(r.sourcePrepared&&r.selectedSource!=&a.selectedCollision)||
                   (!r.sourcePrepared&&r.selectedSource)||
                   r.links[0].size()!=s.count.membership||r.links[1].size()!=s.count.membership||
                   r.entries[0].size()!=s.count.membership||r.entries[1].size()!=s.count.membership||
                   r.memberships[0].attached||r.memberships[1].attached||
                   r.globals.terrainIdentity348!=terrainId||r.globals.identity[1]!=a.movement->filter.identity||
                   r.globals.collisionTypeTag!=a.main.collisionTypeTag||
                   r.globals.movementCategory3E0720!=a.main.materialMask||
                   !plannedIdentityFree(r.globals.identity[0])||!plannedIdentityFree(r.globals.identity[1])||
                   r.globals.identity[0]==mainId||r.globals.identity[0]==secondaryId||r.globals.identity[0]==terrainId||
                   r.globals.identity[1]==mainId||r.globals.identity[1]==secondaryId||r.globals.identity[1]==terrainId||
                   r.globals.identity[0]==r.globals.identity[1])
                    return fail(d,"actual selected recovery backing differs from the same RA query route");
            }
            for(const auto& w:inputs_.wreck){
                if(!w.assets->prepared()||w.queries->prepared()||w.queries->constructed())
                    return fail(d,"wreck owners changed before actual Follow backing construction");
                std::size_t extent=0;
                if(!meshExtent(w.assets->targetE4().mesh,extent)||extent>wreckMeshStack_)
                    return fail(d,"actual wreck mesh exceeds sealed capacity");
                for(const auto id:w.identities)if(id==mainId||id==secondaryId||id==terrainId||
                   (a.recovery&&(id==a.recovery->globals.identity[0]||id==a.recovery->globals.identity[1])))
                    return fail(d,"planned wreck identity conflicts with actual arena query owners");
            }
            // No reset/write to the actual application normal-mode owner. A real
            // completed query, not cold construction, establishes its source word.
            actual_=std::make_unique<ActualQueryOwners>(a);
            if(shared)return true; //Only actual arena custody; no query constructor claimed.
            follow_.reset(new original_follow_query_bridge::QueryBridge(s,candidates_->decodedSource()->world,
                a.normalMode,a.main,a.mainFilter,a.secondary,a.secondaryFilter,a.terrain.metadata,
                a.recovery,a.recovery?&a.frame:nullptr,a.movement,recoveryMeshStack_,wreckMeshStack_));
            queriesConstructed_=true;return true;
        }catch(...){return fail(d,"actual Follow construction failed; storage prefix retained");}
    }
#include "ra_shared_owner_lease50la.inl"
#include "ra_shared_role_views50la.inl"
#include "ra_shared_cold_attachment50la.inl"
#include "ra_native_repulsor_replacement50la.inl"
#include "ra_shared_fresh_attachment50la.inl"
#include "ra_shared_direct_pod50la.inl"
#include "ra_shared_contact50la.inl"
#include "ra_shared_terrain_prepare50la.inl"
#include "ra_shared_terrain_result50la.inl"
#include "ra_shared_projection50la.inl"
#include "ra_shared_selection50la.inl"
#include "ra_shared_terrain_nohit50la.inl"
#include "ra_shared_engine_gravity50la.inl"
#include "ra_shared_engine_application50la.inl"
#include "ra_shared_aero_phases50la.inl"
#include "ra_shared_spring_response50la.inl"
#include "ra_shared_terrain_response50la.inl"
#include "ra_shared_secondary_forces50la.inl"
#include "ra_shared_secondary_joint_prefix50la.inl"
    //Uncalled until the terrain POD consumer reaches its real owner9C read.
    //The typed token names the actual racer, never the separate340 body.
    bool readNativePodRacerOwner9C(const SharedOwnerLease& lease,std::size_t slot,
        g::NativeRacerTerrainIdentity50LA& value)const noexcept {
        if(!sharedLeaseCurrent(lease)||slot>=stagedRacerTerrain_.size()||
           !lease.producer_->bodyRegistry)return false;
        const auto owner=stagedRacerTerrain_[slot].lock();
        if(!owner||!nativeRacerTerrainCurrent(*lease.producer_->activeSession,slot,*owner))return false;
        const auto& s=*storage_.get();
        if(slot>=s.sharedRoster_->count)return false;
        const auto& row=s.sharedRoster_->objects[slot*storage::Storage::rosterRoles+
            static_cast<std::size_t>(storage::Storage::RosterRole::pod694)];
        if(!row||!row->coldBound||row->slot!=slot||row->generation!=owner->generation_||
           row->bodyA0!=stagedPods694_[slot].bodyA0||!row->membership.state.attached||
           row->membership.state.object!=row->reservedRef)return false;
        const auto actual=lease.producer_->bodyRegistry->racerTerrainIdentity(slot,*owner);
        if(!actual.value)return false;value=actual;return true;
    }
    bool registerWorldOwners(native_racer50HN::Bank& bank,
        const std::shared_ptr<OwnershipBridge>& self,Diagnostic& d)noexcept {
        d={};
        // This is the legacy candidate-lane/World Bank path. Shared-mode
        // candidates and their F8/copied lanes must never enter this snapshot.
        // No shared-mode Bank pin/borrow is issued before Stage C is complete.
        if(storage_.get()&&storage_.get()->sharedRoster_)
            return fail(d,"shared roster Bank publication requires typed owner integration");
        if(terminal_||closed_||registrationAttempted_||!queriesConstructed_||
           self.get()!=this||bankIdentity_||bank.raWorldLifetime_||
           bank.retired_||bank.sealed_||bank.visiting_||bank.readBlocked_)
            return fail(d,"RA world registration requires its actual fresh bridge and unsealed idle Bank");
        registrationAttempted_=true;
        try {
            // Pin first, before even the first candidate lane can escape. It
            // remains installed if any later span allocation/overlap rejects.
            bank.raWorldLifetime_=std::make_shared<BankPin>(bank,self);
            bankLifetime_=bank.raWorldLifetime_;bankIdentity_=&bank;
            for(const auto& binding:*candidates_->staticBindings()){
                auto& collision=binding.candidate->candidate.collision;
                bank.owner(collision.filter.flagsF8);
                bank.owner(collision.copied40);
                bank.owner(collision.copied50);
            }
            storage_.get()->registerMutableOwners(bank);
            worldOwnersRegistered_=true;return true;
        }catch(...){return fail(d,"RA world registration failed; Bank pin and partial spans retained until destruction");}
    }
    bool triggerBankCurrent(const native_racer50HN::Bank& bank,std::size_t slot,bool inVisit)const noexcept {
        const auto pin=bankLifetime_.lock();
        if(!pin||bankIdentity_!=&bank||pin->identity!=&bank||
           bank.raWorldLifetime_.get()!=pin.get()||!bank.sealed_||bank.count_!=inputs_.collisionRoster.size()||
           !candidates_->triggerActions_||!actual_)return false;
        // visit(0) uses readBlocked rather than the swapping lease; batched
        // visits use visiting for every slot, including zero.
        if(inVisit?(!bank.visiting_&&!(slot==0u&&bank.readBlocked_)):
            (bank.visiting_||bank.readBlocked_))return false;
        return !inVisit||(slot<inputs_.collisionRoster.size()&&actual_->mainFilter.identity==bodyIdentities_[slot].main&&
            actual_->secondaryFilter.identity==bodyIdentities_[slot].secondary&&
            actual_->main.resource==&bodyIdentities_[slot].collision->main()&&
            actual_->secondary.resource==&bodyIdentities_[slot].collision->secondary());
    }
    bool observeTriggerPoses(Diagnostic& d)noexcept {
        if(!candidates_->triggerActions_||!actual_)return fail(d,"RA trigger owner/query absent");
        auto& q=*actual_;auto& actions=*candidates_->triggerActions_;
        if(!actions.observePrepared(q.mainFilter.identity,*q.main.resource,q.main.worldMatrix)||
           !actions.observePrepared(q.secondaryFilter.identity,*q.secondary.resource,q.secondary.worldMatrix))
            return fail(d,"RA trigger pose observation rejected");
        return true;
    }
    bool cameraBankCurrent()const noexcept {
        // Check the live nonforgeable control block BEFORE dereferencing Bank.
        const auto pin=bankLifetime_.lock();
        return pin&&bankIdentity_&&pin->identity==bankIdentity_&&
            bankIdentity_->raWorldLifetime_.get()==pin.get()&&bankIdentity_->sealed_&&
            bankIdentity_->count_==inputs_.collisionRoster.size()&&!bankIdentity_->visiting_&&!bankIdentity_->readBlocked_;
    }
    // Every course saves the racer frame without the process-shared motion and
    // 308030 noise (one layout: rr::game::RacerSharedFrameLayout50GB).
    bool frameLanesRegistered(const g::VehicleFrameState49W& frame)const noexcept {
        using L=g::RacerSharedFrameLayout50GB;const auto* b=reinterpret_cast<const unsigned char*>(&frame);
        return registeredRange(b,L::motionBegin)&&registeredRange(b+L::motionEnd,L::noiseBegin-L::motionEnd)&&
            registeredRange(b+L::noiseEnd,sizeof(frame)-L::noiseEnd);
    }
    bool registeredRange(const void* address,std::size_t bytes)const noexcept {
        const auto first=reinterpret_cast<std::uintptr_t>(address);
        for(const auto& span:bankIdentity_->spans_){
            const auto start=reinterpret_cast<std::uintptr_t>(span.address);
            if(first>=start&&first-start<=span.bytes&&bytes<=span.bytes-(first-start))return true;
        }
        return false;
    }
    static bool coldRoleMatches(const g::VehicleFrameState49W& frame,
        const g::AiRuntimeState50IV& ai,const native_racer50HW::Rig& rig,
        std::size_t slot,std::uint32_t humanCount)noexcept {
        if(humanCount>1u||slot>=8u)return false;
        const auto* identity=g::original_racer_selection50IJ::findByPodId(rig.podId);
        if(!identity)return false;
        if(slot<humanCount)return frame.fields.vehicle58==0u&&!frame.fields.nativeAiOwnersBound50IV&&!ai.initialized;
        return frame.fields.vehicle58==1u&&frame.fields.nativeAiOwnersBound50IV&&ai.initialized&&
            ai.managerIndexFE0==slot&&ai.racer60==identity->racer60;
    }
    bool gridInputsRegistered(const GridArenaOwners& a)const noexcept {
        const auto& q=*actual_;const auto& frame=q.frame;
        // Actual AI registration deliberately excludes the shared noise lane.
        constexpr auto noise=offsetof(g::VehicleFrameState49W,noise);
        constexpr auto after=noise+sizeof(g::CoupledNoiseContext49M);
        const auto* bytes=reinterpret_cast<const unsigned char*>(&frame);
        return a.humanCount<=1u&&a.humanCount==inputs_.humanCount&&frameLanesRegistered(frame)&&
            registeredRange(&a.bindings,sizeof(a.bindings))&&registeredRange(&a.parameters,sizeof(a.parameters))&&
            registeredRange(&a.selectors,sizeof(a.selectors))&&
            registeredRange(&a.ai,sizeof(a.ai))&&registeredRange(&a.history,sizeof(a.history))&&
            registeredRange(&q.main,sizeof(q.main))&&registeredRange(&q.secondary,sizeof(q.secondary))&&
            registeredRange(&q.mainFilter.identity,sizeof(q.mainFilter.identity))&&
            registeredRange(&q.secondaryFilter.identity,sizeof(q.secondaryFilter.identity))&&
            registeredRange(&q.terrain.initial,sizeof(q.terrain.initial))&&
            registeredRange(&q.terrain.lowerEndpoint140,sizeof(q.terrain.lowerEndpoint140))&&
            registeredRange(&q.terrain.receipt,sizeof(q.terrain.receipt))&&
            registeredRange(&q.terrain.prepared,sizeof(q.terrain.prepared))&&
            registeredRange(&q.terrainLive,sizeof(q.terrainLive));
    }
    static bool prepareGridQuery(g::Type2StaticQuery49X& query,const g::BodyMatrix48G& scene,
        g::LiveZoneBounds50EN& bounds)noexcept {
        if(!query.resource||!query.resource->nodeCount||(query.resource->root&0x8000u)||
           query.resource->root>=query.resource->nodeCount)return false;
        const auto& root=query.resource->nodes[query.resource->root];
        g::QuerySpherePrepareInput48O input{};input.preparedScene=scene;input.shapeType=2;input.hasPairTree=true;
        for(unsigned i=0;i<3;++i){input.treeCenter.v[i]=root.center[i];input.treeHalf.v[i]=root.half[i];}
        g::QuerySpherePrepareOutput48O prepared{};
        if(g::prepareQuerySphere48O(input,prepared)!=g::QuerySpherePrepareStatus48O::Ready)return false;
        query.worldMatrix=prepared.world;
        for(unsigned i=0;i<3;++i)query.sphere.center.v[i]=prepared.center.v[i];
        query.sphere.radius=prepared.radius;
        bounds={};bounds.bounds.shapeTag=2;bounds.bounds.shapeWorld=prepared.world;
        bounds.bounds.cachedCenter=prepared.center;bounds.bounds.cachedRadius=prepared.radius;
        bounds.type2Resource=query.resource;return true;
    }
    bool bindPodLiveQuery(native_racer50HN::Bank& bank,std::size_t slot,
        g::VehicleFrameBindings49W& bindings,g::VehicleFrameParameters49W& parameters,Diagnostic& d,
        std::uint32_t humanCount=0)noexcept {
        d={};
        if(terminal_||closed_||cameraAttempted_||!worldOwnersRegistered_||!actual_||!actual_->recovery||
           !cameraBankCurrent()||&bank!=bankIdentity_||slot>=inputs_.collisionRoster.size()||humanCount>1u||humanCount!=inputs_.humanCount||
           (podContext_&&podHumanCount_!=humanCount)||gridAttempted_[slot]||podBindingInstalled_[slot]||
           !registeredRange(&bindings,sizeof(bindings))||!registeredRange(&parameters,sizeof(parameters))||
           (podBindings_&&podBindings_!=&bindings)||(podParameters_&&podParameters_!=&parameters))
            return fail(d,"RA pod binding requires same actual cold Bank/arena before final grid");
        constexpr auto noise=offsetof(g::VehicleFrameState49W,noise);
        constexpr auto after=noise+sizeof(g::CoupledNoiseContext49M);
        const auto* frame=reinterpret_cast<const unsigned char*>(&actual_->frame);
        (void)frame;(void)after;
        if(!frameLanesRegistered(actual_->frame))
            return fail(d,"actual frame lanes are not registered with this Bank");
        if(gridArena_&&(&gridArena_->bindings!=&bindings||&gridArena_->parameters!=&parameters))
            return fail(d,"actual pod arena differs from retained grid arena");
        auto& s=*storage_.get();auto& recovery=*actual_->recovery;
        if(!s.count.podQuery||s.count.podQuery>s.lifecycle.objectCount||
           !registeredRange(&recovery.memberships[0],sizeof(recovery.memberships[0])))
            return fail(d,"actual reserved694 membership is not registered");
        try {
            // Retain before any Bank-saved pointer publication. On a partial
            // failure this object remains until actual Bank destruction.
            if(!podContext_){
                podContext_.reset(new g::PodLiveQueryContext50IZ{
                    {s.lifecycle,s.world,recovery.memberships[0],s.membershipWork,s.count.podQuery},&parameters.shape});
                podBindings_=&bindings;podParameters_=&parameters;podHumanCount_=humanCount;
            }
            bool complete=false;
            bank.visit(slot,[&](){auto& q=*actual_;const auto& r=*q.recovery;
                if(q.frame.phase!=g::VehicleFramePhase49W::Ready||q.frame.bodies.main.frameFactor!=0.f||
                   (slot<humanCount?(q.frame.fields.vehicle58!=0u||q.frame.fields.nativeAiOwnersBound50IV):
                       (q.frame.fields.vehicle58!=1u||!q.frame.fields.nativeAiOwnersBound50IV))||
                   !r.sourcePrepared||!r.selectedSource||!r.selectedSource->prepared()||
                   q.main.resource!=&r.selectedSource->main()||q.secondary.resource!=&r.selectedSource->secondary()||
                   q.frame.shape.registrationE0||r.memberships[0].attached||r.memberships[1].attached||
                   (bindings.podLiveQuery50IZ&&bindings.podLiveQuery50IZ!=podContext_.get())){
                    fail(d,"actual per-slot selected recovery/694 binding is not cold");return;}
                const auto& source=r.selectedSource->mainTarget().source;
                if(std::memcmp(source.local40.v,parameters.shape.local40.v,sizeof source.local40.v)||
                   std::memcmp(source.rootCenter.v,parameters.shape.rootCenter.v,sizeof source.rootCenter.v)||
                   std::memcmp(source.rootHalf.v,parameters.shape.rootHalf.v,sizeof source.rootHalf.v)||
                   !g::validVehicleSceneCount49W(q.frame,bindings)){
                    fail(d,"actual registered parameters/bindings differ from selected source");return;}
                bindings.podLiveQuery50IZ=podContext_.get();podBindingInstalled_[slot]=true;complete=true;
            });
            return complete;
        }catch(...){return fail(d,"RA pod binding exception; context and saved slot pointer retained");}
    }
    bool placeGrid(native_racer50HN::Bank& bank,std::size_t slot,
        const GridArenaOwners& input,Diagnostic& d,bool canonicalTournamentGrid=false)noexcept {
        return placeGridMode(bank,slot,input,d,canonicalTournamentGrid,false);
    }
    bool placeSharedGrid(native_racer50HN::Bank& bank,std::size_t slot,
        const GridArenaOwners& input,Diagnostic& d,bool canonicalTournamentGrid=false)noexcept {
        return placeGridMode(bank,slot,input,d,canonicalTournamentGrid,true);
    }
    bool placeGridMode(native_racer50HN::Bank& bank,std::size_t slot,
        const GridArenaOwners& input,Diagnostic& d,bool canonicalTournamentGrid,bool shared)noexcept {
        d={};
        if(!fixedRecoveryRouteAllowed())
            return fail(d,"actual source auxiliary queries cannot adopt fixed native recovery arrays");
        if(!storage_.get()||shared!=bool(storage_.get()->sharedRoster_))
            return fail(d,"cold grid mode differs from actual fresh storage ownership");
        if(shared&&(sharedEffectsStopped_||sharedOwnerLease_||sharedRoleViews_||nativeFifoPins_||
           triggerQueryActive_||(endCandidateProducer_&&endCandidateProducer_->inCall)))
            return fail(d,"shared cold grid has outstanding source/query custody");
        if(terminal_||closed_||cameraAttempted_||!worldOwnersRegistered_||!actual_||
           !cameraBankCurrent()||&bank!=bankIdentity_||slot>=inputs_.collisionRoster.size()||gridAttempted_[slot]||
           inputs_.collisionRoster.size()!=bank.count()||!gridInputsRegistered(input)||
           (rosterResources_&&!rosterSlotComplete_[slot]))
            return fail(d,"RA cold grid requires the same sealed active-roster Bank and registered actual arena");
        if(canonicalTournamentGrid&&(input.humanCount!=1u||bank.count()!=8u))
            return fail(d,"Tournament grid requires the actual one-human seven-AI roster");
        if(actual_->recovery&&(!podBindingInstalled_[slot]||podHumanCount_!=input.humanCount||podBindings_!=&input.bindings||podParameters_!=&input.parameters))
            return fail(d,"RA recovery grid requires the actual previously bound pod context");
        try {
            if(gridArena_){
                const auto& a=*gridArena_;
                if(&a.bindings!=&input.bindings||&a.parameters!=&input.parameters||&a.selectors!=&input.selectors||&a.ai!=&input.ai||
                   &a.history!=&input.history||&a.rigs!=&input.rigs||a.humanCount!=input.humanCount)
                    return fail(d,"RA grid arena borrows cannot be replaced");
            }else{
                // Validate every active identity lane before any placement. Each
                // visited slot separately proves its actual Ready constructor.
                const auto idMain=bank.bindRead(actual_->mainFilter.identity);
                const auto idSecondary=bank.bindRead(actual_->secondaryFilter.identity);
                for(std::size_t i=0;i<inputs_.collisionRoster.size();++i){
                    if(!input.rigs[i])return fail(d,"RA grid requires every active retained Rig owner");
                    const auto m=bank.read(i,idMain),s=bank.read(i,idSecondary);
                    const auto terrain=actual_->terrain.metadata.filter.identity;
                    if(m==terrain||s==terrain)return fail(d,"RA grid body identity conflicts with terrain query");
                    if(inputs_.recovery)for(const auto id:inputs_.recovery->globals.identity)
                        if(m==id||s==id)return fail(d,"RA grid body identity conflicts with recovery query");
                    for(const auto& wreck:inputs_.wreck)for(const auto id:wreck.identities)
                        if(m==id||s==id)return fail(d,"RA grid body identity conflicts with reserved wreck query");
                    if(!m||!s||m==s||!plannedIdentityFree(m)||!plannedIdentityFree(s))
                        return fail(d,"RA grid body identities conflict with actual world");
                    for(unsigned j=0;j<i;++j)if(m==bank.read(j,idMain)||m==bank.read(j,idSecondary)||
                        s==bank.read(j,idMain)||s==bank.read(j,idSecondary))
                        return fail(d,"RA grid requires distinct actual per-slot body identities");
                }
                std::size_t stack=0;
                for(const auto& collision:candidates_->decodedSource()->collisions){std::size_t extent=0;
                    if(!meshExtent(collision.mesh,extent))return fail(d,"RA ground scratch extent overflow");
                    stack=std::max(stack,extent);}
                groundStack_.resize(stack);
                gridArena_=std::make_unique<GridArenaOwners>(input);
                for(std::size_t i=0;i<inputs_.collisionRoster.size();++i)gridRigIdentity_[i]=input.rigs[i].get();
            }
            for(std::size_t i=0;i<inputs_.collisionRoster.size();++i)if(input.rigs[i].get()!=gridRigIdentity_[i])
                return fail(d,"actual retained grid Rig owner was replaced");
            gridAttempted_[slot]=true;
            bool complete=false;
            bank.visit(slot,[&](){
                auto& q=*actual_;auto& state=q.frame;auto& a=*gridArena_;const auto& rig=*a.rigs[slot];
                auto& storage=*storage_.get();
                std::array<storage::Storage::SharedRosterObject*,storage::Storage::rosterRoles> rows{};
                if(shared&&!sharedColdRowsCurrent(slot,rows)){
                    fail(d,"actual shared grid role owners differ");return;}
                if(shared){
                    if(!rows[3]->podContext||a.bindings.podLiveQuery50IZ!=&*rows[3]->podContext){
                        fail(d,"actual saved per-slot POD context differs");return;}
                    for(unsigned i=0;i<3;++i)if(rows[i]->membership.state.attached||rows[i]->membership.state.object||
                        storage.objects[rows[i]->reservedRef-1u].identity||
                        rows[i]->effectRevision==std::numeric_limits<std::uint64_t>::max()){
                        fail(d,"shared grid requires its genuinely unpublished constructor rows");return;}
                }
                auto& mainMember=shared?rows[0]->membership:storage.mainMembership;
                auto& secondaryMember=shared?rows[1]->membership:storage.secondaryMembership;
                auto& terrainMember=shared?rows[2]->membership:storage.terrainMembership;
                const auto mainRef=shared?rows[0]->reservedRef:storage.count.mainQuery;
                const auto secondaryRef=shared?rows[1]->reservedRef:storage.count.secondaryQuery;
                const auto terrainRef=shared?rows[2]->reservedRef:storage.count.terrainQuery;
                const auto main=static_cast<unsigned>(a.bindings.lifecycle.main190);
                const auto physical=static_cast<unsigned>(a.bindings.lifecycle.physical19C);
                if(q.recovery&&!shared&&a.bindings.podLiveQuery50IZ!=podContext_.get()){
                    fail(d,"selected slot lost its actual bridge-owned pod context");return;}
                const native_racer_collision50HW::Owner* collision=nullptr;
                for(const auto* item:inputs_.collisionRoster)if(q.main.resource==&item->main()&&
                    q.secondary.resource==&item->secondary())collision=item;
                if(!collision||state.phase!=g::VehicleFramePhase49W::Ready||state.bodies.main.frameFactor!=0.f||
                   !coldRoleMatches(state,a.ai,rig,slot,a.humanCount)||
                   collision->binding().archivePath!=rig.resourceFile.string()||
                   !g::validVehicleSceneCount49W(state,a.bindings)||main>=state.sceneNodeCount||
                   physical>=state.sceneNodeCount||a.bindings.lifecycle.main190!=rig.main||
                   a.bindings.lifecycle.physical19C!=rig.physical||
                   (rig.modelCount!=3u&&rig.modelCount!=5u)||
                   q.main.filter!=&q.mainFilter||q.secondary.filter!=&q.secondaryFilter||
                   q.main.collisionTypeTag!=candidates_->staticBindings()->front().candidate->candidate.collision.typeTag||
                   q.secondary.collisionTypeTag!=q.main.collisionTypeTag||
                   !q.main.normalMode54Bound||q.main.normalMode54!=0u||
                   !q.secondary.normalMode54Bound||q.secondary.normalMode54!=0u||
                   !q.terrain.prepared||!q.terrain.metadata.normalMode54Bound||q.terrain.metadata.normalMode54!=1u||
                   q.terrain.receipt.archiveBytes!=collision->receipt().archiveBytes||
                   q.terrain.receipt.podBoxMBytes!=collision->receipt().memberBytes[0]||
                   std::bit_cast<std::uint32_t>(q.terrain.receipt.authoredLift)!=
                       std::bit_cast<std::uint32_t>(a.parameters.visual.verticalLimit7EC)||
                   !live_terrain_probe50fr::matches(q.terrainLive,q.terrain.initial)){
                    fail(d,"actual Ready racer-role/rig/COL/PHY/query ownership differs at grid slot");return;}
                constexpr std::uint32_t hostRank[8]={4,0,1,2,3,5,6,7};
                g::GridCenterResult50AC grid{};
                // Preserve Bank identity; original1AE1D8 /1AE400 select start rank only.
                const auto rank=canonicalTournamentGrid?
                    (slot?static_cast<std::uint32_t>(slot-1):7u):hostRank[slot];
                if(!source_grid50IG::prepare(candidates_->decodedSource()->map,rank,grid)){
                    fail(d,"actual RA M-spline grid geometry rejected");return;}
                auto requested=state.nodes[main].world100;
                for(unsigned axis=0;axis<3;++axis){const auto value=std::bit_cast<float>(grid.position[axis]);
                    requested.v[12+axis]=axis==1?value+a.parameters.visual.verticalLimit7EC:value;}
                g::BodyMatrix48G grounded{};
                if(!native_racer50HN::ColdGroundProbe50HN::run(candidates_->decodedSource()->collisions,
                    requested,a.parameters.visual.verticalLimit7EC,groundStack_.data(),groundStack_.size(),grounded)){
                    fail(d,"actual RA collision ground probe rejected");return;}
                gridRetirementRequired_[slot]=true;
                for(auto* membership:{&mainMember,&secondaryMember,&terrainMember})
                    if(g::removeLiveZoneMembership50EN(storage.lifecycle,storage.world,membership->state,
                        storage.membershipWork).status!=g::LiveZoneStatus50EN::Complete){
                        fail(d,"RA grid detach incomplete; actual partial effects retained");return;}
                if(!native_racer50HN::placeColdGrid(state,a.bindings,grounded,rig.rebaseModels,rig.modelCount,a.history)){
                    fail(d,"actual cold model/body grid translation rejected");return;}
                g::LiveZoneBounds50EN mainBounds{},secondaryBounds{};
                if(!prepareGridQuery(q.main,state.nodes[main].world100,mainBounds)||
                   !prepareGridQuery(q.secondary,state.nodes[physical].world100,secondaryBounds)){
                    fail(d,"actual grid query cache construction rejected");return;}
                // Source1AE588/1AE590 ->2169C0 chooses first genuine C34 ref1.
                if(g::attachLiveZoneMembership50EN(storage.lifecycle,storage.world,mainMember.state,
                    mainMember.storage(),mainRef,1,g::ZoneMethod50CL::Second,0.f,
                    mainBounds,storage.membershipWork).status!=g::LiveZoneStatus50EN::Complete||
                   (shared&&!publishSharedColdGridRole(*rows[0],mainBounds))||
                   g::attachLiveZoneMembership50EN(storage.lifecycle,storage.world,secondaryMember.state,
                    secondaryMember.storage(),secondaryRef,1,g::ZoneMethod50CL::Second,0.f,
                    secondaryBounds,storage.membershipWork).status!=g::LiveZoneStatus50EN::Complete||
                   (shared&&!publishSharedColdGridRole(*rows[1],secondaryBounds))){
                    fail(d,"RA grid body attachment incomplete; actual effects retained");return;}
                g::TerrainPrepareInput48M terrain{};terrain.state=q.terrain.initial;
                terrain.scene=state.nodes[main].world100;terrain.lowerEndpoint140=q.terrain.lowerEndpoint140;
                g::TerrainPrepareState48M prepared{};
                if(g::prepareTerrainSinglePair48M(terrain,prepared)!=g::TerrainPrepareStatus48M::QueryReady){
                    fail(d,"actual grid terrain cache rejected");return;}
                const auto bounds=live_terrain_probe50fr::bounds(shared?*rows[2]->terrainResource:q.terrainLive,prepared);
                if(g::attachLiveZoneMembership50EN(storage.lifecycle,storage.world,terrainMember.state,
                    terrainMember.storage(),terrainRef,1,g::ZoneMethod50CL::Second,0.f,
                    bounds,storage.membershipWork).status!=g::LiveZoneStatus50EN::Complete||
                   (shared&&!publishSharedColdGridRole(*rows[2],bounds))){
                    fail(d,"RA grid terrain attachment incomplete; actual effects retained");return;}
                auto& receipt=gridCompletion_[slot];
                receipt.main=state.nodes[main].world100;receipt.physical=state.nodes[physical].world100;
                receipt.mainResource=q.main.resource;receipt.secondaryResource=q.secondary.resource;
                receipt.mainRevision=mainMember.state.revision;
                receipt.secondaryRevision=secondaryMember.state.revision;
                receipt.terrainRevision=terrainMember.state.revision;
                if(candidates_->triggerActions_&&!observeTriggerPoses(d))return;
                complete=true;
            });
            gridCompletion_[slot].complete=complete;
            return complete;
        }catch(...){return fail(d,"RA grid exception; Bank saved actual partial slot effects");}
    }
    bool recoveryLanesRegistered(const FreshRecoveryArenaOwners& a,bool shared=false)const noexcept {
        if(!fixedRecoveryRouteAllowed())return false;
        const auto& r=*actual_->recovery;const auto& m=*actual_->movement;
        if(!registeredRange(a.endpoints,sizeof(a.endpoints))||!registeredRange(a.metadata,sizeof(a.metadata))||
           !registeredRange(&m.copied40,sizeof(m.copied40))||!registeredRange(&m.copied50,sizeof(m.copied50))||
           !registeredRange(&m.filter.flagsF8,sizeof(m.filter.flagsF8))||
           !registeredRange(&m.initial,sizeof(m.initial))||
           !registeredRange(&m.initialQueryCurrent,sizeof(m.initialQueryCurrent))||
           !registeredRange(&r.podFilter.flagsF8,sizeof(r.podFilter.flagsF8))||
           !registeredRange(&r.podCopied40,sizeof(r.podCopied40))||!registeredRange(&r.podCopied50,sizeof(r.podCopied50))||
           !registeredRange(&r.movementTag,sizeof(r.movementTag))||!registeredRange(&r.movementSphere,sizeof(r.movementSphere))||
           !registeredRange(&r.receipt,sizeof(r.receipt))||!registeredRange(&r.selectedSource,sizeof(r.selectedSource))||
           !registeredRange(&r.sourcePrepared,sizeof(r.sourcePrepared)))return false;
        const auto& ids=r.podFilter.identityLists[0].identities;
        if(!ids.empty()&&!registeredRange(ids.data(),ids.size()*sizeof(ids[0])))return false;
        if(!shared)for(unsigned i=0;i<2;++i)
            if(!registeredRange(&r.memberships[i],sizeof(r.memberships[i]))||
               !registeredRange(r.links[i].data(),r.links[i].size()*sizeof(r.links[i][0]))||
               !registeredRange(r.entries[i].data(),r.entries[i].size()*sizeof(r.entries[i][0])))return false;
        return a.source.endpoints==a.endpoints&&a.source.endpointCount==2&&
            a.source.sceneMetadata==a.metadata&&a.source.sceneMetadataCount==2;
    }
    undertrack_owners50ew_fixture::BorrowedQueries recoveryBorrows()noexcept {
        auto& a=*actual_;auto& s=*storage_.get();auto& r=*freshRecovery_;
        undertrack_owners50ew_fixture::BorrowedQueries q{};
        q.main690=&a.main;q.mainMembership=&s.mainMembership.state;
        q.mainStorage=s.mainMembership.storage();q.mainRef=s.count.mainQuery;
        q.movement7A4=a.movement;q.movementEndpoints=r.endpoints;q.movementMetadata=r.metadata;q.movementSource=&r.source;
        q.terrain348=&a.terrain.initial;q.terrainResource=&a.terrainLive.resource;q.terrainMetadata=&a.terrain.metadata;
        q.terrainMembership=&s.terrainMembership.state;q.terrainStorage=s.terrainMembership.storage();q.terrainRef=s.count.terrainQuery;
        return q;
    }
    bool attachFresh(native_racer50HN::Bank& bank,std::size_t slot,
        const std::shared_ptr<g::original_gc_gameplay_lighting_candidate::Owner>& light,
        const FreshRecoveryArenaOwners* recovery,Diagnostic& d)noexcept {
        return attachFreshMode(bank,slot,light,recovery,d,false);
    }
    bool attachSharedFresh(native_racer50HN::Bank& bank,std::size_t slot,
        const std::shared_ptr<g::original_gc_gameplay_lighting_candidate::Owner>& light,
        const FreshRecoveryArenaOwners& recovery,Diagnostic& d)noexcept {
        return attachFreshMode(bank,slot,light,&recovery,d,true);
    }
    bool attachFreshMode(native_racer50HN::Bank& bank,std::size_t slot,
        const std::shared_ptr<g::original_gc_gameplay_lighting_candidate::Owner>& light,
        const FreshRecoveryArenaOwners* recovery,Diagnostic& d,bool shared)noexcept {
        d={};
        if(!fixedRecoveryRouteAllowed())
            return fail(d,"actual source auxiliary queries cannot adopt fixed native recovery arrays");
        if(!storage_.get()||shared!=bool(storage_.get()->sharedRoster_))
            return fail(d,"fresh attachment mode differs from actual World ownership");
        if(shared&&(!recovery||sharedEffectsStopped_||sharedOwnerLease_||sharedRoleViews_||nativeFifoPins_||
           triggerQueryActive_||(endCandidateProducer_&&endCandidateProducer_->inCall)))
            return fail(d,"shared fresh attachment has outstanding source/query custody");
        if(terminal_||closed_||cameraAttempted_||!cameraBankCurrent()||&bank!=bankIdentity_||slot>=inputs_.collisionRoster.size()||
           !actual_||!gridArena_||!gridCompletion_[slot].complete||freshAttempted_[slot]||!light||
           (freshLighting_&&freshLighting_!=light)||bool(recovery)!=bool(actual_->recovery))
            return fail(d,"RA fresh attachment requires actual completed grid and same cold owners");
        if(!light->matchesRaFreshRoster(static_cast<std::uint32_t>(bank.count())))
            return fail(d,"same authenticated RA lighting owner lacks the actual Bank roster");
        if(actual_->recovery&&(!podBindingInstalled_[slot]||podBindings_!=&gridArena_->bindings||
           podParameters_!=&gridArena_->parameters))
            return fail(d,"RA fresh recovery requires its real retained pod context binding");
        if(recovery&&(!actual_->movement||!recoveryLanesRegistered(*recovery,shared)))
            return fail(d,"actual recovery endpoint/source or Bank lanes unavailable");
        if(freshRecovery_&&(&freshRecovery_->endpoints!=&recovery->endpoints||
           &freshRecovery_->metadata!=&recovery->metadata||&freshRecovery_->source!=&recovery->source))
            return fail(d,"actual recovery arena cannot be replaced");
        try {
            if(recovery&&!freshRecovery_)freshRecovery_=std::make_unique<FreshRecoveryArenaOwners>(*recovery);
            freshLighting_=light;freshAttempted_[slot]=true;
            bool complete=false;
            bank.visit(slot,[&](){
                auto& q=*actual_;auto& a=*gridArena_;auto& s=*storage_.get();auto& f=q.frame;
                const auto& proof=gridCompletion_[slot];
                std::array<storage::Storage::SharedRosterObject*,storage::Storage::rosterRoles> sharedRows{};
                if(shared&&!sharedColdRowsCurrent(slot,sharedRows)){
                    fail(d,"actual shared fresh role ownership differs");return;}
                auto& mainMember=shared?sharedRows[0]->membership:s.mainMembership;
                auto& secondaryMember=shared?sharedRows[1]->membership:s.secondaryMembership;
                auto& terrainMember=shared?sharedRows[2]->membership:s.terrainMembership;
                if(shared&&(!sharedRows[3]->podContext||a.bindings.podLiveQuery50IZ!=&*sharedRows[3]->podContext)){
                    fail(d,"actual per-slot saved POD context differs before visibility");return;}
                if(q.recovery&&!shared&&a.bindings.podLiveQuery50IZ!=podContext_.get()){
                    fail(d,"actual saved pod context pointer was replaced");return;}
                const auto main=static_cast<unsigned>(a.bindings.lifecycle.main190);
                const auto physical=static_cast<unsigned>(a.bindings.lifecycle.physical19C);
                if(!a.rigs[slot]||a.rigs[slot].get()!=gridRigIdentity_[slot]||
                   f.phase!=g::VehicleFramePhase49W::Ready||f.bodies.main.frameFactor!=0.f||
                   !coldRoleMatches(f,a.ai,*a.rigs[slot],slot,a.humanCount)||
                   main>=f.sceneNodeCount||physical>=f.sceneNodeCount||
                   q.main.resource!=proof.mainResource||q.secondary.resource!=proof.secondaryResource||
                   std::memcmp(&proof.main,&f.nodes[main].world100,sizeof(proof.main))||
                   std::memcmp(&proof.physical,&f.nodes[physical].world100,sizeof(proof.physical))||
                   !mainMember.state.attached||!secondaryMember.state.attached||!terrainMember.state.attached||
                   mainMember.state.revision!=proof.mainRevision||
                   secondaryMember.state.revision!=proof.secondaryRevision||terrainMember.state.revision!=proof.terrainRevision){
                    fail(d,"actual final placement changed before fresh attachment");return;}
                const native_racer_collision50HW::Owner* selected=nullptr;
                for(const auto* owner:inputs_.collisionRoster)
                    if(q.main.resource==&owner->main()&&q.secondary.resource==&owner->secondary())selected=owner;
                if(!selected||selected->binding().archivePath!=a.rigs[slot]->resourceFile.string()||
                   a.bindings.lifecycle.main190!=a.rigs[slot]->main||
                   a.bindings.lifecycle.physical19C!=a.rigs[slot]->physical){
                    fail(d,"actual selected Rig/collision source changed after grid");return;}
                const auto& authored=selected->mainTarget().source;
                if(std::memcmp(authored.local40.v,a.parameters.shape.local40.v,sizeof authored.local40.v)||
                   std::memcmp(authored.rootCenter.v,a.parameters.shape.rootCenter.v,sizeof authored.rootCenter.v)||
                   std::memcmp(authored.rootHalf.v,a.parameters.shape.rootHalf.v,sizeof authored.rootHalf.v)){
                    fail(d,"actual selected694 authoring differs from source owner");return;}
                if(recovery){
                    auto& r=*q.recovery;
                    auto& podMember=shared?sharedRows[3]->membership.state:r.memberships[0];
                    auto& movementMember=shared?sharedRows[4]->membership.state:r.memberships[1];
                    const auto podRef=shared?sharedRows[3]->reservedRef:s.count.podQuery;
                    const auto movementRef=shared?sharedRows[4]->reservedRef:s.count.movementQuery;
                    if(!r.sourcePrepared||!r.selectedSource||!r.selectedSource->prepared()||
                       q.main.resource!=&r.selectedSource->main()||q.secondary.resource!=&r.selectedSource->secondary()||
                       r.selectedSource->binding().archivePath!=a.rigs[slot]->resourceFile.string()||
                       r.globals.collisionTypeTag!=q.main.collisionTypeTag||
                       r.globals.identity[0]!=r.podFilter.identity||r.globals.identity[1]!=q.movement->filter.identity||
                       q.movement->collisionTypeTag!=q.main.collisionTypeTag||
                       podMember.attached||movementMember.attached||
                       !podRef||!movementRef||podRef==movementRef||
                       podRef>s.objects.size()||movementRef>s.objects.size()||
                       s.objects[podRef-1].identity||s.objects[movementRef-1].identity){
                        fail(d,"actual selected recovery or reserved collection rows differ");return;}
                    const auto& source=r.selectedSource->mainTarget().source;
                    if(std::memcmp(source.local40.v,a.parameters.shape.local40.v,sizeof source.local40.v)||
                       std::memcmp(source.rootCenter.v,a.parameters.shape.rootCenter.v,sizeof source.rootCenter.v)||
                       std::memcmp(source.rootHalf.v,a.parameters.shape.rootHalf.v,sizeof source.rootHalf.v)||
                       std::bit_cast<std::uint32_t>(r.receipt.maxLiftHeight)!=
                       std::bit_cast<std::uint32_t>(a.parameters.visual.verticalLimit7EC)){
                        fail(d,"selected recovery source shape/lift changed");return;}
                }
                // Before the actual694 prefix or any membership can mutate.
                freshRetirementRequired_[slot]=true;
                g::original_gc_gameplay_lighting_candidate::Diagnostic ld{};
                if(!freshLighting_->attachFreshVehicle(static_cast<std::uint32_t>(slot),f,a.bindings,a.parameters,ld)){
                    fail(d,"actual fresh7F0 attachment failed; prefix retained");return;}
                if(recovery){
                    auto& r=*q.recovery;
                    if(shared){
                        if(!attachSharedFreshQueries(sharedRows,slot,d))return;
                    }else {
                    if(!undertrack_owners50ew_fixture::attachFreshQueries(r,f,a.bindings,a.parameters,recoveryBorrows(),
                        {s.count.podQuery,s.count.movementQuery},s.lifecycle,s.world,s.membershipWork,1,
                        slot<a.humanCount?undertrack_owners50ew_fixture::ColdQueryRole50LH::humanPlayer:
                            undertrack_owners50ew_fixture::ColdQueryRole50LH::originalAi)){
                        fail(d,"actual fresh694/7A4 attachment incomplete");return;}
                    g::CollisionSphere48J sphere{};
                    std::memcpy(sphere.center.v,f.shape.center60.v,sizeof sphere.center.v);sphere.radius=f.shape.radius80;
                    const auto pod=g::projectCollectionObject50CF(r.podFilter,sphere,q.main.collisionTypeTag,0,
                        std::bit_cast<std::uint32_t>(f.shape.center60.v[3]));
                    const auto movement=g::projectCollectionObject50CF(q.movement->filter,r.movementSphere,
                        q.movement->collisionTypeTag,r.movementTag,
                        std::bit_cast<std::uint32_t>(r.memberships[1].bounds.cachedCenter.v[3]));
                    if(!pod.valid||!movement.valid){fail(d,"actual recovery projection rejected");return;}
                    const g::CollectionProjection50CF* rows[]{&pod,&movement};
                    const g::CollectionRef50BX refs[]{s.count.podQuery,s.count.movementQuery};
                    for(unsigned i=0;i<2;++i){const auto index=refs[i]-1u;s.objects[index]=rows[i]->object;
                        s.source[index].flagsF8=rows[i]->state.flagsF8;s.source[index].cachedCenter=rows[i]->state.cachedCenter;
                        s.source[index].cachedRadius=rows[i]->state.cachedRadius;}
                    }
                }
                complete=true;
            });
            freshComplete_[slot]=complete;return complete;
        }catch(...){return fail(d,"RA fresh attachment exception; saved partial effects retained");}
    }
    bool retireFresh(native_racer50HN::Bank& bank,std::size_t slot,Diagnostic& d)noexcept {
        d={};
        if(nativeFifoPins_||closed_||!cameraBankCurrent()||&bank!=bankIdentity_||slot>=inputs_.collisionRoster.size()||
           (camera_&&camera_->session&&!camera_->session->closed_)||
           (follow_&&follow_->attempted_&&!follow_->closed_)){
            d.message="RA fresh retirement requires real camera/Follow cleanup and live Bank";return false;}
        terminal_=true;
        for(auto& record:gridCompletion_)record.complete=false;
        freshComplete_.fill(false);
        if(!freshRetirementRequired_[slot])return true;
        try {
            bool complete=false;
            bank.visit(slot,[&](){auto& s=*storage_.get();auto& q=*actual_;
                if(q.recovery){auto& r=*q.recovery;
                    const g::CollectionRef50BX refs[]{s.count.podQuery,s.count.movementQuery};
                    for(unsigned i=0;i<2;++i){const auto ref=refs[i];
                        if(!ref||ref>s.objects.size()||
                           (s.objects[ref-1].identity&&s.objects[ref-1].identity!=r.globals.identity[i])||
                           (r.memberships[i].attached&&r.memberships[i].object!=ref)||
                           (!i&&q.frame.shape.registrationE0&&q.frame.shape.registrationE0!=ref)){
                            d.message="recovery teardown ownership mismatch; backing retained";return;}
                        if(g::removeLiveZoneMembership50EN(s.lifecycle,s.world,r.memberships[i],s.membershipWork).status!=
                           g::LiveZoneStatus50EN::Complete){d.message="actual recovery removal incomplete";return;}
                        if(!i)q.frame.shape.registrationE0=0;
                        // Exact reserved row only, after its real removal. Bank
                        // visit saves this slot; other saved slots remain owned.
                        s.objects[ref-1]={};s.source[ref-1]={};
                    }
                }
                g::original_gc_gameplay_lighting_candidate::Diagnostic ld{};
                if(!freshLighting_||!freshLighting_->retireRaFreshVehicle(static_cast<std::uint32_t>(slot),ld)){
                    d.message="actual visibility removal incomplete; same light owner retained";return;}
                complete=true;
            });
            if(complete)freshRetirementRequired_[slot]=false;
            return complete;
        }catch(...){d.message="RA fresh teardown exception; actual effects retained for retry";return false;}
    }
    bool retireGrid(native_racer50HN::Bank& bank,std::size_t slot,Diagnostic& d)noexcept {
        d={};
        // Cleanup deliberately remains callable after terminal partial failure.
        if(nativeFifoPins_||closed_||!cameraBankCurrent()||&bank!=bankIdentity_||slot>=inputs_.collisionRoster.size()||
           (camera_&&camera_->session&&!camera_->session->closed_)||
           (follow_&&follow_->attempted_&&!follow_->closed_)){
            d.message="RA grid retirement requires its live Bank after actual camera/Follow cleanup";return false;}
        gridCompletion_[slot].complete=false;freshComplete_[slot]=false;
        if(freshRetirementRequired_[slot]){
            d.message="actual fresh visibility/recovery must retire before grid membership";return false;}
        if(!gridRetirementRequired_[slot])return true;
        try {
            bool complete=false;
            bank.visit(slot,[&](){auto& s=*storage_.get();
                for(auto* membership:{&s.mainMembership,&s.secondaryMembership,&s.terrainMembership})
                    if(g::removeLiveZoneMembership50EN(s.lifecycle,s.world,membership->state,
                        s.membershipWork).status!=g::LiveZoneStatus50EN::Complete){
                        d.message="actual RA grid removal incomplete; retry with same Bank and backing";return;}
                if(s.mainMembership.state.attached||s.secondaryMembership.state.attached||
                   s.terrainMembership.state.attached){d.message="actual grid memberships remain attached";return;}
                complete=true;
            });
            if(complete)gridRetirementRequired_[slot]=false;
            return complete;
        }catch(...){d.message="RA grid cleanup exception; actual slot effects and retirement obligation retained";return false;}
    }
    original_camera_session50KX::Result prepareCamera(std::uint64_t generation,Diagnostic& d)noexcept {
        namespace c=original_camera_session50KX;
        d={};
        if(terminal_||closed_||cameraPrepareTerminal_||!camera_||!camera_->session||
           !cameraBankCurrent()||!generation||generation!=camera_->generation){
            d.message="RA camera requires the same live Bank and actual manager/session generation";
            d.camera.status=c::Status::owner;return d.camera;
        }
        if(!freshLighting_||camera_->lighting!=freshLighting_||
           std::find(freshComplete_.begin(),freshComplete_.end(),false)!=freshComplete_.end()){
            d.message="RA camera requires every actual fresh attachment from its same lighting owner";
            d.camera.status=c::Status::owner;return d.camera;
        }
        if(!camera_->fog.current()||!camera_->display||
           camera_->display->trackFog3EC!=&camera_->fog.fields_){
            d.message="actual retained RA fog owner was replaced";
            cameraPrepareTerminal_=true;d.camera.status=c::Status::owner;return d.camera;}
        const auto& rig=*camera_->selectedRig;
        if(rig.phy!=camera_->phySnapshot||
           reinterpret_cast<const char*>(rig.phy.data())!=camera_->authored.phy||
           rig.phy.size()!=camera_->authored.phyBytes){
            d.message="selected original camera PHY owner changed";
            cameraPrepareTerminal_=true;d.camera.status=c::Status::authoring;return d.camera;
        }
        try {
            d.camera=camera_->session->prepare(generation,camera_->authored,*camera_->display);
            if(d.camera.status!=c::Status::complete)d.message="actual SourceCamera preparation remains incomplete";
            return d.camera;
        }catch(...){cameraPrepareTerminal_=true;d.message="actual SourceCamera prepare exception; partial state retained";
            d.camera.status=c::Status::terminal;return d.camera;}
    }
    original_camera_session50KX::Result closeCamera(bool abandon,Diagnostic& d)noexcept {
        namespace c=original_camera_session50KX;
        d={};
        if(nativeFifoPins_){d.message="actual collector FIFO still retains SourceCamera";
            d.camera.status=c::Status::owner;return d.camera;}
        if(!camera_){d.camera.status=c::Status::complete;return d.camera;}
        if(!camera_->session){camera_.reset();d.camera.status=c::Status::complete;return d.camera;}
        if(!cameraBankCurrent()){
            d.message="actual SourceCamera retirement requires its still-live idle Bank";
            d.camera.status=c::Status::owner;return d.camera;
        }
        try {
            d.camera=abandon?camera_->session->abandonAndCloseSpatial():camera_->session->close();
            // Inspect the actual algorithm's closed field, never infer it from
            // a success-shaped return or caller claim. Failed cleanup retries.
            if(camera_->session->closed_)camera_.reset();
            else d.message="actual SourceCamera cleanup incomplete; camera/light/pool retained";
            return d.camera;
        }catch(...){d.message="actual SourceCamera cleanup exception; owners retained";
            d.camera.status=c::Status::terminal;return d.camera;}
    }
    bool canClose()const noexcept {
        if(sourceIncomingZonesAttempted_&&!sourceIncomingZonesWithdrawn_)return false;
        if(endCandidateProducer_&&endCandidateProducer_->inCall)return false;
        if(sharedOwnerLease_||sharedRoleViews_||sharedEffectsStopped_)return false;
        if(nativeFifoPins_||triggerQueryActive_)return false;
        if(closed_)return true;
        if(rosterRetirementRequired_)return false;
        if(!bankLifetime_.expired())return false;
        // Borrowed arena must still exist. Only real Bank destruction proves
        // no saved slot can restore a pointer after terminal unbinding.
        const bool published=std::find(podBindingInstalled_.begin(),podBindingInstalled_.end(),true)!=podBindingInstalled_.end();
        if(published&&podBindings_&&podBindings_->podLiveQuery50IZ&&podBindings_->podLiveQuery50IZ!=podContext_.get()&&
            !sharedPodContextOwned(podBindings_->podLiveQuery50IZ))return false;
        for(const bool pending:freshRetirementRequired_)if(pending)return false;
        for(const bool pending:gridRetirementRequired_)if(pending)return false;
        if(camera_&&camera_->session&&!camera_->session->closed_)return false;
        // Actual bank0 integration must successfully retire actual query
        // attempts before this cold owner may disappear. Do not invoke close
        // outside bank0 merely because the aggregate is being destroyed.
        if(follow_&&follow_->attempted_&&!follow_->closed_)return false;
        const auto* s=storage_.get();
        if(s&&s->sharedRoster_){
            // Shared query/Follow borrowers still require later actual retirement;
            // inspect owners, not a caller-supplied readiness flag. The typed
            // arena alone may close only after the real Bank pin has expired.
            if(follow_||camera_||podContext_||endCandidateProducer_||queriesConstructed_||
               sharedOwnerLease_||sharedEffectsStopped_||!s->sharedRosterCanRetire())return false;
        }
        if(s&&(s->mainMembership.state.attached||s->secondaryMembership.state.attached||
               s->terrainMembership.state.attached))return false;
        if(storageConstructed_&&inputs_.recovery&&
           (inputs_.recovery->memberships[0].attached||inputs_.recovery->memberships[1].attached))return false;
        // An entirely unpublished cold allocation can cancel its native
        // borrows at terminal close. Any source-published piece still needs
        // its real destructor/Entity retirement, even after membership detach.
        if(s)for(const auto& w:inputs_.wreck)if(w.queries&&w.queries->prepared()&&
            w.queries->worldBinding().world==&s->world&&
            !w.queries->canReleaseUnpublishedWorld(s->world))return false;
        return true;
    }
    bool close()noexcept {
        if(closed_)return true;if(sceneOwner_||!canClose())return false;
        if(auto* s=storage_.get();s&&s->sharedRoster_){
            //Only after the real Bank pin and all lexical/source borrowers
            //have retired. Clear solely an ACTUAL owned row-context pointer.
            if(podBindings_&&sharedPodContextOwned(podBindings_->podLiveQuery50IZ))
                podBindings_->podLiveQuery50IZ=nullptr;
            //Revoke real immutable policy borrows before membership backing
            //can disappear. A retained Context policy can no longer select it.
            if(s->nativeDetectorPolicy_&&!s->nativeDetectorPolicy_->revokeShared(*s))return false;
            if(!s->retireSharedRosterAllocation())return false;
            actual_.reset(); //No saved Bank or lexical source borrower survives canClose.
        }
        // Only after ALL bank/camera/scene/registry lifetime gates above pass.
        // A later failure retains Storage; already cancelled borrows stay
        // terminal and are skipped on retry. No successful source retirement
        // callback or caller-provided completion flag is accepted.
        if(auto* s=storage_.get())for(const auto& w:inputs_.wreck)if(w.queries&&w.queries->prepared()&&
            w.queries->worldBinding().world==&s->world&&
            !w.queries->releaseUnpublishedWorld(s->world))return false;
        // Native unbound cold backing has no source pieces to destruct. Only
        // after every actual query withdrawal may its stable reservations die.
        if(rosterResources_&&rosterResources_->wreckBacking_&&
           !rosterResources_->wreckBacking_->releaseUnboundColdReservations())return false;
        inputs_.wreck.clear(); // No dangling query aliases survive native release.
        if(podBindings_&&podBindings_->podLiveQuery50IZ==podContext_.get())podBindings_->podLiveQuery50IZ=nullptr;
        podContext_.reset();podBindings_=nullptr;podParameters_=nullptr;
        endCandidateProducer_=nullptr; //Revoke Context borrow before owned backing disappears.
        camera_.reset();follow_.reset();gridArena_.reset();closed_=true;return true;
        // Storage, lease, typed borrows and source die in reverse member order
        // with this owner. No registry is replaced/reused after close.
    }
public:
    OwnershipBridge(const OwnershipBridge&)=delete;OwnershipBridge& operator=(const OwnershipBridge&)=delete;
    OwnershipBridge(OwnershipBridge&&)=delete;OwnershipBridge& operator=(OwnershipBridge&&)=delete;
    ~OwnershipBridge(){if(!close())std::terminate();}
};
} // namespace ra_source_camera50LA

#pragma once
#include "native_racer_cables50hw_fixture.h"
namespace original_main_entry50KG {class NativeCollectorFifo50LA;}
namespace original_cable_selection50LA { class Binding; }
#include "ra_static_candidates50la.h"
#include "native_racer_bank50hn_fixture.h"
#include "ra_source_camera_inputs50la.h"
#include "ra_track_fog50la.h"
#include "ra_static_fog_observation50la.h"
#include "original_follow_query_bridge50kx.h"
#include "terrain_probe_authoring50fq_fixture.h"
#include "native_racer_grid50hn_fixture.h"
#include "source_grid50ig_fixture.h"
#include "../../src/game/query_prepare48o.h"
#include "../../src/game/collection_projection50cf.h"
#include "../../src/game/pod_live_query50iz.h"
#include <cstddef>
#include <algorithm>
#include <exception>
#include <memory>
#include <limits>
#include <vector>

namespace ra_cold_aggregate50LA { class Owner; }
namespace ra_pod_scene50LA { class Owner; }
namespace ra_cold_roster50LA { class Resources; class Operation; }
namespace ra_static_lighting50LA { class Binding; }
namespace ra_static_fog50LA { class Binding; }
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
    g::AiRuntimeState50IV& ai;
    g::BasePoseHistoryState50Y& history;
    const std::array<std::unique_ptr<native_racer50HW::Rig>,8>& rigs;
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
    friend class ::ra_pod_scene50LA::Owner;
    const ::ra_pod_scene50LA::Owner* sceneOwner_=nullptr;
    friend class ::original_main_entry50KG::NativeCollectorFifo50LA;
    friend class ::original_cable_selection50LA::Binding;
    friend class ra_cold_aggregate50LA::Owner;
    friend class ra_cold_roster50LA::Operation;
    std::shared_ptr<const ra_cold_roster50LA::Resources> rosterResources_;
    const terrain_probe_authoring50fq_fixture::Owner* rosterTerrain_=nullptr;
    const g::Type3QueryInput48P* rosterMovement_=nullptr;
    std::array<std::uint32_t,3> rosterAuxiliaryIds_{}; // terrain,694,7A4
    struct BodyIdentityReservation {
        const native_racer_collision50HW::Owner* collision=nullptr;
        std::uint32_t main=0,secondary=0;
    };
    std::array<BodyIdentityReservation,8> bodyIdentities_{};
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
    bool cameraAttempted_=false,cameraPrepareTerminal_=false;
    bool storageAttempted_=false,capacitySealed_=false,storageConstructed_=false;
    bool bindAttempted_=false,queriesConstructed_=false,terminal_=false,closed_=false;
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
        ~BankPin() override {
            // Bank's slots/spans have already died. Do not touch Bank here.
            // SourceCamera must have performed its real bank0 close first.
            owner->verifyFollowRetiredForBankDestruction();
        }
    };
    void verifyFollowRetiredForBankDestruction()const noexcept {
        if(nativeFifoPins_||triggerQueryActive_)std::terminate();
        if(rosterRetirementRequired_)std::terminate();
        for(const bool pending:freshRetirementRequired_)if(pending)std::terminate();
        for(const bool pending:gridRetirementRequired_)if(pending)std::terminate();
        if(camera_&&camera_->session&&!camera_->session->closed_)std::terminate();
        if(follow_&&follow_->attempted_&&!follow_->closed_)std::terminate();
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
    bool constructStorage(std::shared_ptr<const ra_static_candidates50LA::Owner> candidates,
        const RuntimeQueryCapacityInputs& input,Diagnostic& d)noexcept {
        d={};
        if(storageAttempted_||terminal_||closed_)return fail(d,"RA runtime storage cannot be replayed");
        storageAttempted_=true;
        try {
            const auto* source=candidates?candidates->decodedSource():nullptr;
            const auto* bindings=candidates?candidates->staticBindings():nullptr;
            if(!source||!bindings||source->collisions.size()!=178u||bindings->size()!=(candidates->triggerActions_?180u:178u)||
               input.collisionRoster.empty()||input.collisionRoster.size()>8u||
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
            const std::size_t base=6u+(inputs_.recovery?2u:0u);
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
                for(auto* target:{&row.main,&row.secondary}){
                    while(next&&!available(next)){
                        if(next==std::numeric_limits<std::uint32_t>::max())next=0;else ++next;
                    }
                    if(!next)return fail(d,"RA native body identity namespace exhausted");
                    *target=next;
                    if(next==std::numeric_limits<std::uint32_t>::max())next=0;else ++next;
                }
            }
            if(candidates_->triggerActions_){
                if(inputs_.collisionRoster.size()!=8u||!rosterResources_)
                    return fail(d,"RA triggers require the actual all-eight retained roster");
                std::array<ra_trigger_actions50LA::RacerIdentities,8> ids{};
                for(std::size_t i=0;i<8u;++i)ids[i]={bodyIdentities_[i].main,bodyIdentities_[i].secondary};
                // This retained RA route constructs eight original AI racers.
                // No human-prefix0 fallback or camera-derived exclusion.
                if(!candidates_->triggerActions_->prepare(*candidates_->triggerSource_,ids,0u,source->ai.track.count))
                    return fail(d,"RA original trigger identity/action preparation failed");
            }
            capacitySealed_=true;
            auto lease=ra_cold_sources50LA::Owner::leaseLiveWorld(candidates_->source_);
            if(!lease)return fail(d,"same-source RA live world already leased or invalid");
            const storage::SourceWorldView<ra_cold_sources50LA::CollisionOwner> view{
                source->world,{source->collisions.data(),source->collisions.size()}};
            if(!storage_.prepareSource(view,bindings->data(),bindings->size(),limits_,d.storage,
                &lease->world(),lease))return fail(d,"RA runtime static/query storage construction failed");
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
            follow_.reset(new original_follow_query_bridge::QueryBridge(s,candidates_->decodedSource()->world,
                a.normalMode,a.main,a.mainFilter,a.secondary,a.secondaryFilter,a.terrain.metadata,
                a.recovery,a.recovery?&a.frame:nullptr,a.movement,recoveryMeshStack_,wreckMeshStack_));
            queriesConstructed_=true;return true;
        }catch(...){return fail(d,"actual Follow construction failed; storage prefix retained");}
    }
    bool registerWorldOwners(native_racer50HN::Bank& bank,
        const std::shared_ptr<OwnershipBridge>& self,Diagnostic& d)noexcept {
        d={};
        if(terminal_||closed_||registrationAttempted_||!queriesConstructed_||
           self.get()!=this||bankIdentity_||bank.raWorldLifetime_||
           bank.sealed_||bank.visiting_||bank.readBlocked_)
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
           bank.raWorldLifetime_.get()!=pin.get()||!bank.sealed_||bank.count_!=8u||
           !candidates_->triggerActions_||!actual_)return false;
        // visit(0) uses readBlocked rather than the swapping lease; batched
        // visits use visiting for every slot, including zero.
        if(inVisit?(!bank.visiting_&&!(slot==0u&&bank.readBlocked_)):
            (bank.visiting_||bank.readBlocked_))return false;
        return !inVisit||(slot<8u&&actual_->mainFilter.identity==bodyIdentities_[slot].main&&
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
            bankIdentity_->count_==8u&&!bankIdentity_->visiting_&&!bankIdentity_->readBlocked_;
    }
    bool registeredRange(const void* address,std::size_t bytes)const noexcept {
        const auto first=reinterpret_cast<std::uintptr_t>(address);
        for(const auto& span:bankIdentity_->spans_){
            const auto start=reinterpret_cast<std::uintptr_t>(span.address);
            if(first>=start&&first-start<=span.bytes&&bytes<=span.bytes-(first-start))return true;
        }
        return false;
    }
    bool gridInputsRegistered(const GridArenaOwners& a)const noexcept {
        const auto& q=*actual_;const auto& frame=q.frame;
        // Actual AI registration deliberately excludes the shared noise lane.
        constexpr auto noise=offsetof(g::VehicleFrameState49W,noise);
        constexpr auto after=noise+sizeof(g::CoupledNoiseContext49M);
        const auto* bytes=reinterpret_cast<const unsigned char*>(&frame);
        return registeredRange(bytes,noise)&&registeredRange(bytes+after,sizeof(frame)-after)&&
            registeredRange(&a.bindings,sizeof(a.bindings))&&registeredRange(&a.parameters,sizeof(a.parameters))&&
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
        g::VehicleFrameBindings49W& bindings,g::VehicleFrameParameters49W& parameters,Diagnostic& d)noexcept {
        d={};
        if(terminal_||closed_||cameraAttempted_||!worldOwnersRegistered_||!actual_||!actual_->recovery||
           !cameraBankCurrent()||&bank!=bankIdentity_||slot>=8u||gridAttempted_[slot]||podBindingInstalled_[slot]||
           !registeredRange(&bindings,sizeof(bindings))||!registeredRange(&parameters,sizeof(parameters))||
           (podBindings_&&podBindings_!=&bindings)||(podParameters_&&podParameters_!=&parameters))
            return fail(d,"RA pod binding requires same actual cold Bank/arena before final grid");
        constexpr auto noise=offsetof(g::VehicleFrameState49W,noise);
        constexpr auto after=noise+sizeof(g::CoupledNoiseContext49M);
        const auto* frame=reinterpret_cast<const unsigned char*>(&actual_->frame);
        if(!registeredRange(frame,noise)||!registeredRange(frame+after,sizeof(actual_->frame)-after))
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
                podBindings_=&bindings;podParameters_=&parameters;
            }
            bool complete=false;
            bank.visit(slot,[&](){auto& q=*actual_;const auto& r=*q.recovery;
                if(q.frame.phase!=g::VehicleFramePhase49W::Ready||q.frame.bodies.main.frameFactor!=0.f||
                   q.frame.fields.vehicle58!=1u||!q.frame.fields.nativeAiOwnersBound50IV||
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
        const GridArenaOwners& input,Diagnostic& d)noexcept {
        d={};
        if(terminal_||closed_||cameraAttempted_||!worldOwnersRegistered_||!actual_||
           !cameraBankCurrent()||&bank!=bankIdentity_||slot>=8u||gridAttempted_[slot]||
           inputs_.collisionRoster.size()!=8u||!gridInputsRegistered(input)||
           (rosterResources_&&!rosterSlotComplete_[slot]))
            return fail(d,"RA cold grid requires the same sealed eight-slot Bank and registered actual arena");
        if(actual_->recovery&&(!podBindingInstalled_[slot]||podBindings_!=&input.bindings||podParameters_!=&input.parameters))
            return fail(d,"RA recovery grid requires the actual previously bound pod context");
        try {
            if(gridArena_){
                const auto& a=*gridArena_;
                if(&a.bindings!=&input.bindings||&a.parameters!=&input.parameters||&a.ai!=&input.ai||
                   &a.history!=&input.history||&a.rigs!=&input.rigs)
                    return fail(d,"RA grid arena borrows cannot be replaced");
            }else{
                // Validate all eight identity lanes before any placement. Each
                // visited slot separately proves its actual Ready constructor.
                const auto idMain=bank.bindRead(actual_->mainFilter.identity);
                const auto idSecondary=bank.bindRead(actual_->secondaryFilter.identity);
                for(unsigned i=0;i<8;++i){
                    if(!input.rigs[i])return fail(d,"RA grid requires eight retained actual Rig owners");
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
                for(unsigned i=0;i<8;++i)gridRigIdentity_[i]=input.rigs[i].get();
            }
            for(unsigned i=0;i<8;++i)if(input.rigs[i].get()!=gridRigIdentity_[i])
                return fail(d,"actual retained grid Rig owner was replaced");
            gridAttempted_[slot]=true;
            bool complete=false;
            bank.visit(slot,[&](){
                auto& q=*actual_;auto& state=q.frame;auto& a=*gridArena_;const auto& rig=*a.rigs[slot];
                auto& storage=*storage_.get();
                const auto main=static_cast<unsigned>(a.bindings.lifecycle.main190);
                const auto physical=static_cast<unsigned>(a.bindings.lifecycle.physical19C);
                if(q.recovery&&a.bindings.podLiveQuery50IZ!=podContext_.get()){
                    fail(d,"selected slot lost its actual bridge-owned pod context");return;}
                const native_racer_collision50HW::Owner* collision=nullptr;
                for(const auto* item:inputs_.collisionRoster)if(q.main.resource==&item->main()&&
                    q.secondary.resource==&item->secondary())collision=item;
                if(!collision||state.phase!=g::VehicleFramePhase49W::Ready||state.bodies.main.frameFactor!=0.f||
                   state.fields.vehicle58!=1u||!state.fields.nativeAiOwnersBound50IV||!a.ai.initialized||
                   a.ai.managerIndexFE0!=slot||rig.podId.size()!=5u||a.ai.racer60>99u||
                   rig.podId!=std::string("POD")+char('0'+a.ai.racer60/10u)+char('0'+a.ai.racer60%10u)||
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
                    fail(d,"actual Ready AI/rig/COL/PHY/query ownership differs at grid slot");return;}
                constexpr std::uint32_t hostRank[8]={4,0,1,2,3,5,6,7};
                g::GridCenterResult50AC grid{};
                if(!source_grid50IG::prepare(candidates_->decodedSource()->map,hostRank[slot],grid)){
                    fail(d,"actual RA M-spline grid geometry rejected");return;}
                auto requested=state.nodes[main].world100;
                for(unsigned axis=0;axis<3;++axis){const auto value=std::bit_cast<float>(grid.position[axis]);
                    requested.v[12+axis]=axis==1?value+a.parameters.visual.verticalLimit7EC:value;}
                g::BodyMatrix48G grounded{};
                if(!native_racer50HN::ColdGroundProbe50HN::run(candidates_->decodedSource()->collisions,
                    requested,a.parameters.visual.verticalLimit7EC,groundStack_.data(),groundStack_.size(),grounded)){
                    fail(d,"actual RA collision ground probe rejected");return;}
                gridRetirementRequired_[slot]=true;
                for(auto* membership:{&storage.mainMembership,&storage.secondaryMembership,&storage.terrainMembership})
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
                if(g::attachLiveZoneMembership50EN(storage.lifecycle,storage.world,storage.mainMembership.state,
                    storage.mainMembership.storage(),storage.count.mainQuery,1,g::ZoneMethod50CL::Second,0.f,
                    mainBounds,storage.membershipWork).status!=g::LiveZoneStatus50EN::Complete||
                   g::attachLiveZoneMembership50EN(storage.lifecycle,storage.world,storage.secondaryMembership.state,
                    storage.secondaryMembership.storage(),storage.count.secondaryQuery,1,g::ZoneMethod50CL::Second,0.f,
                    secondaryBounds,storage.membershipWork).status!=g::LiveZoneStatus50EN::Complete){
                    fail(d,"RA grid body attachment incomplete; actual effects retained");return;}
                g::TerrainPrepareInput48M terrain{};terrain.state=q.terrain.initial;
                terrain.scene=state.nodes[main].world100;terrain.lowerEndpoint140=q.terrain.lowerEndpoint140;
                g::TerrainPrepareState48M prepared{};
                if(g::prepareTerrainSinglePair48M(terrain,prepared)!=g::TerrainPrepareStatus48M::QueryReady){
                    fail(d,"actual grid terrain cache rejected");return;}
                const auto bounds=live_terrain_probe50fr::bounds(q.terrainLive,prepared);
                if(g::attachLiveZoneMembership50EN(storage.lifecycle,storage.world,storage.terrainMembership.state,
                    storage.terrainMembership.storage(),storage.count.terrainQuery,1,g::ZoneMethod50CL::Second,0.f,
                    bounds,storage.membershipWork).status!=g::LiveZoneStatus50EN::Complete){
                    fail(d,"RA grid terrain attachment incomplete; actual effects retained");return;}
                auto& receipt=gridCompletion_[slot];
                receipt.main=state.nodes[main].world100;receipt.physical=state.nodes[physical].world100;
                receipt.mainResource=q.main.resource;receipt.secondaryResource=q.secondary.resource;
                receipt.mainRevision=storage.mainMembership.state.revision;
                receipt.secondaryRevision=storage.secondaryMembership.state.revision;
                receipt.terrainRevision=storage.terrainMembership.state.revision;
                if(candidates_->triggerActions_&&!observeTriggerPoses(d))return;
                complete=true;
            });
            gridCompletion_[slot].complete=complete;
            return complete;
        }catch(...){return fail(d,"RA grid exception; Bank saved actual partial slot effects");}
    }
    bool recoveryLanesRegistered(const FreshRecoveryArenaOwners& a)const noexcept {
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
        for(unsigned i=0;i<2;++i)
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
        d={};
        if(terminal_||closed_||cameraAttempted_||!cameraBankCurrent()||&bank!=bankIdentity_||slot>=8u||
           !actual_||!gridArena_||!gridCompletion_[slot].complete||freshAttempted_[slot]||!light||
           (freshLighting_&&freshLighting_!=light)||bool(recovery)!=bool(actual_->recovery))
            return fail(d,"RA fresh attachment requires actual completed grid and same cold owners");
        if(!light->matchesRaFreshRoster(static_cast<std::uint32_t>(bank.count())))
            return fail(d,"same authenticated RA lighting owner lacks the actual Bank roster");
        if(actual_->recovery&&(!podBindingInstalled_[slot]||podBindings_!=&gridArena_->bindings||
           podParameters_!=&gridArena_->parameters))
            return fail(d,"RA fresh recovery requires its real retained pod context binding");
        if(recovery&&(!actual_->movement||!recoveryLanesRegistered(*recovery)))
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
                if(q.recovery&&a.bindings.podLiveQuery50IZ!=podContext_.get()){
                    fail(d,"actual saved pod context pointer was replaced");return;}
                const auto main=static_cast<unsigned>(a.bindings.lifecycle.main190);
                const auto physical=static_cast<unsigned>(a.bindings.lifecycle.physical19C);
                if(!a.rigs[slot]||a.rigs[slot].get()!=gridRigIdentity_[slot]||
                   f.phase!=g::VehicleFramePhase49W::Ready||f.bodies.main.frameFactor!=0.f||
                   f.fields.vehicle58!=1u||!f.fields.nativeAiOwnersBound50IV||!a.ai.initialized||a.ai.managerIndexFE0!=slot||
                   main>=f.sceneNodeCount||physical>=f.sceneNodeCount||
                   q.main.resource!=proof.mainResource||q.secondary.resource!=proof.secondaryResource||
                   std::memcmp(&proof.main,&f.nodes[main].world100,sizeof(proof.main))||
                   std::memcmp(&proof.physical,&f.nodes[physical].world100,sizeof(proof.physical))||
                   !s.mainMembership.state.attached||!s.secondaryMembership.state.attached||!s.terrainMembership.state.attached||
                   s.mainMembership.state.revision!=proof.mainRevision||
                   s.secondaryMembership.state.revision!=proof.secondaryRevision||s.terrainMembership.state.revision!=proof.terrainRevision){
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
                    if(!r.sourcePrepared||!r.selectedSource||!r.selectedSource->prepared()||
                       q.main.resource!=&r.selectedSource->main()||q.secondary.resource!=&r.selectedSource->secondary()||
                       r.selectedSource->binding().archivePath!=a.rigs[slot]->resourceFile.string()||
                       r.globals.collisionTypeTag!=q.main.collisionTypeTag||
                       r.globals.identity[0]!=r.podFilter.identity||r.globals.identity[1]!=q.movement->filter.identity||
                       q.movement->collisionTypeTag!=q.main.collisionTypeTag||
                       r.memberships[0].attached||r.memberships[1].attached||
                       !s.count.podQuery||!s.count.movementQuery||s.count.podQuery==s.count.movementQuery||
                       s.count.podQuery>s.objects.size()||s.count.movementQuery>s.objects.size()||
                       s.objects[s.count.podQuery-1].identity||s.objects[s.count.movementQuery-1].identity){
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
                    if(!undertrack_owners50ew_fixture::attachFreshQueries(r,f,a.bindings,a.parameters,recoveryBorrows(),
                        {s.count.podQuery,s.count.movementQuery},s.lifecycle,s.world,s.membershipWork,1)){
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
                complete=true;
            });
            freshComplete_[slot]=complete;return complete;
        }catch(...){return fail(d,"RA fresh attachment exception; saved partial effects retained");}
    }
    bool retireFresh(native_racer50HN::Bank& bank,std::size_t slot,Diagnostic& d)noexcept {
        d={};
        if(nativeFifoPins_||closed_||!cameraBankCurrent()||&bank!=bankIdentity_||slot>=8u||
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
        if(nativeFifoPins_||closed_||!cameraBankCurrent()||&bank!=bankIdentity_||slot>=8u||
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
        if(nativeFifoPins_||triggerQueryActive_)return false;
        if(closed_)return true;
        if(rosterRetirementRequired_)return false;
        if(!bankLifetime_.expired())return false;
        // Borrowed arena must still exist. Only real Bank destruction proves
        // no saved slot can restore a pointer after terminal unbinding.
        const bool published=std::find(podBindingInstalled_.begin(),podBindingInstalled_.end(),true)!=podBindingInstalled_.end();
        if(published&&podBindings_&&podBindings_->podLiveQuery50IZ&&podBindings_->podLiveQuery50IZ!=podContext_.get())return false;
        for(const bool pending:freshRetirementRequired_)if(pending)return false;
        for(const bool pending:gridRetirementRequired_)if(pending)return false;
        if(camera_&&camera_->session&&!camera_->session->closed_)return false;
        // Actual bank0 integration must successfully retire actual query
        // attempts before this cold owner may disappear. Do not invoke close
        // outside bank0 merely because the aggregate is being destroyed.
        if(follow_&&follow_->attempted_&&!follow_->closed_)return false;
        const auto* s=storage_.get();
        if(s&&(s->mainMembership.state.attached||s->secondaryMembership.state.attached||
               s->terrainMembership.state.attached))return false;
        if(storageConstructed_&&inputs_.recovery&&
           (inputs_.recovery->memberships[0].attached||inputs_.recovery->memberships[1].attached))return false;
        // A prepared piece retains world pointers even after its memberships
        // detach. The later actual piece owner must provide a typed retirement
        // join before such a borrow can outlive this Storage; absent that join,
        // reject close rather than release backing beneath the piece.
        if(s)for(const auto& w:inputs_.wreck)if(w.queries&&w.queries->prepared()&&
            w.queries->worldBinding().world==&s->world)return false;
        return true;
    }
    bool close()noexcept {
        if(closed_)return true;if(sceneOwner_||!canClose())return false;
        if(podBindings_&&podBindings_->podLiveQuery50IZ==podContext_.get())podBindings_->podLiveQuery50IZ=nullptr;
        podContext_.reset();podBindings_=nullptr;podParameters_=nullptr;
        camera_.reset();follow_.reset();closed_=true;return true;
        // Storage, lease, typed borrows and source die in reverse member order
        // with this owner. No registry is replaced/reused after close.
    }
public:
    OwnershipBridge(const OwnershipBridge&)=delete;OwnershipBridge& operator=(const OwnershipBridge&)=delete;
    OwnershipBridge(OwnershipBridge&&)=delete;OwnershipBridge& operator=(OwnershipBridge&&)=delete;
    ~OwnershipBridge(){if(!close())std::terminate();}
};
} // namespace ra_source_camera50LA

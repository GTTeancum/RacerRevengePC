#pragma once
#include "../../src/game/original_application_entities50la.h"
#include "../../src/game/original_native_list_method_custody50la.h"
#include <optional>
#include <new>
#include "authored_runtime_ground50cp_fixture.h"
#include "live_terrain_probe50fr_fixture.h"
#include "native_terrain_result_owner50la.h"
#include "native_repulsor_payload_owner50la.h"
#include "../../src/game/native_racer_terrain_owner50la.h"
#include "../../src/game/native_racer_aero_owner50la.h"
#include "../../src/game/pod_live_query50iz.h"
#include "../../src/game/original_racer_main_body_owner50la.h"
#include "candidate_handles50cb.h"
#include "static_type2_producer49x.h"
#include "collection_projection50cf.h"
#include "vehicle_candidates50ca.h"
#include "native_spring_detector_policy50la.h"
#include "zone_registration_events50cq.h"
#include "type3_query48p.h"
#include "../../src/assets/material_audio50hi.h"
#include <algorithm>
#include <array>
#include <bit>
#include <cstring>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <type_traits>

// Cold ownership only. This replaces size assumptions, not original resources
// or authoring: every candidate must point at one actual loaded COL owner.
// No geometry, filters, opaque identities, spawn, route or query state is made
// up here. The caller constructs those owners before preparing this layer.
namespace ra_pod_scene50LA {class Owner;}
namespace ra_static_registry50LA { class ColdStage; }
namespace ra_source_camera50LA { class OwnershipBridge; }
namespace native_racer_collision50HW { class Owner; }
namespace rr::game { class OriginalRacerMainBodyOwner50LA; }
namespace track_runtime_storage50IG {
using namespace rr::game;
namespace a=rr::assets;
using Ground=authored_runtime_ground50cp_fixture::ColdScene;
// Share the cold loader's one-block ceiling. Every optional actual query
// reference is charged before allocating registry/object backing.
inline constexpr std::size_t hardTerrainOwners=authored_runtime_ground50cp_fixture::hardCollisionOwners50IG;
inline constexpr std::size_t runtimeSearchSlotLimit=authored_runtime_ground50cp_fixture::runtimeSearchSlotLimit50IG;
inline constexpr std::size_t runtimeDynamicReferenceReserve=authored_runtime_ground50cp_fixture::runtimeDynamicReferenceReserve50IG;
inline constexpr std::size_t runtimeRecoveryReferenceReserve=authored_runtime_ground50cp_fixture::runtimeRecoveryReferenceReserve50IG;
static_assert(runtimeDynamicReferenceReserve==3&&runtimeRecoveryReferenceReserve==2&&
    hardTerrainOwners+runtimeDynamicReferenceReserve==runtimeSearchSlotLimit);
inline constexpr std::size_t hardZones=256,hardPortals=4096;
inline constexpr std::size_t sourceSelectionCapacity=CandidateMergeCapacity48S;
static_assert(sourceSelectionCapacity==64);

//Stage S: the retained per-racer collision roster every storage caller
//passes to the ONE shared-roster factory (rows are sized from it).
struct RosterPlan {
    const native_racer_collision50HW::Owner* const* owners=nullptr;
    std::size_t count=0;
};
struct Limits {
    // Stage B switch: the course's retained racer roster. Set by the ONE
    // storage caller (terrain_interactive_world50du.cpp) for every course
    // world owner; when set, prepare() builds the shared layout (one world,
    // every racer's five rows). Null: legacy layout (port stand-in kept for
    // RA's borrowed route and the no-roster/no-recovery route).
    const RosterPlan* roster=nullptr;
    std::size_t terrainOwners=hardTerrainOwners,zones=hardZones,portals=hardPortals;
    // Requested native element payload, including reserved filter elements.
    // Excludes allocator bookkeeping and independently owned mesh/radii bytes.
    std::size_t coldPayloadBytes=32u*1024u*1024u;
    // Only the explicitly selected original-camera route reserves its actual
    // sphere24/ray190 and distinct death120 records. Legacy callers retain three refs.
    bool originalCameraQueries=false;
    // Only the original-AI recovery route reserves missing actual694/7A4 refs.
    // Existing main690, secondary spring and348 retain their IDs. Follow
    // remains the same owner; its trailing native refs follow the new pair.
    bool originalRecoveryQueries=false;
    // Exact count of real source PhysicsObject pieces constructed cold. Each
    // owns E0/E4/E8; reserve no slot for authored-only pending collections.
    // This capacity option does not publish identities or activate queries.
    std::size_t wreckPieceQueries=0;
};
struct Binding {
    // Caller order is authoritative (TA may retain its existing corpus order).
    // Every original loaded owner must occur exactly once, with no duplicates.
    std::size_t originalOwner=0;
    Type2StaticCandidate49X* candidate=nullptr;
    // Additive original TOB/model owner. Not a fake SCB/static geometry row.
    // Caller authenticates the actual source resource and constructor fields.
    bool animated=false;
    std::uint32_t authoredZone=0;
    std::uint8_t materialA9=0;
    ZoneMethod50CL method=ZoneMethod50CL::First;
    bool trigger=false; // authenticated additive TOB Gate sphere, no SCB index
};
struct Counts {
    std::size_t terrain=0,zones=0,portals=0,methods=0,blocks=0,objects=0;
    std::size_t portalLinks=0,membership=0,selected=0,events=0;
    CollectionRef50BX legacyMainQuery=0,legacySecondaryQuery=0,legacyTerrainQuery=0;
    CollectionRef50BX cameraSphere=0,cameraRay=0,cameraDeath=0;
    CollectionRef50BX legacyPodQuery=0,legacyMovementQuery=0;
    std::size_t legacyRecoveryReferences=0;
    std::size_t sharedRosterCount=0; //Fresh typed allocation only; no active objects implied.
    CollectionRef50BX wreckFirstQuery=0;
    std::size_t wreckReferences=0;
};
struct Diagnostic {
    std::string message;
    std::size_t candidate=0,zone=0,requestedPayloadBytes=0;
};

// Runtime mutation can change elements, never backing address or capacity.
// This cannot be resized, copied or moved after construction.
template<class T> class FixedBuffer {
    std::unique_ptr<T[]> values_;
    std::size_t count_=0;
public:
    explicit FixedBuffer(std::size_t count):
        values_(count?std::make_unique<T[]>(count):nullptr),count_(count){}
    FixedBuffer(const FixedBuffer&)=delete;
    FixedBuffer& operator=(const FixedBuffer&)=delete;
    FixedBuffer(FixedBuffer&&)=delete;
    FixedBuffer& operator=(FixedBuffer&&)=delete;
    T* data() noexcept{return values_.get();}
    const T* data()const noexcept{return values_.get();}
    std::size_t size()const noexcept{return count_;}
    T& operator[](std::size_t i) noexcept{return values_[i];}
    const T& operator[](std::size_t i)const noexcept{return values_[i];}
    T* begin() noexcept{return data();}
    T* end() noexcept{return count_?data()+count_:data();}
};

struct Membership {
    FixedBuffer<CollectionZoneLink50BX> links;
    FixedBuffer<LiveZoneEntry50EN> entries;
    LiveZoneMembership50EN state{};
    explicit Membership(std::size_t count):links(count),entries(count){}
    LiveZoneStorage50EN storage() noexcept{return {links.data(),entries.data(),links.size()};}
};

// Own this at a stable address for the complete session. Backing may be exposed
// to the existing serial frame APIs, but never replace descriptors while a
// query, cursor, racer-bank visit or registration operation is outstanding.
class Owner;
struct Storage {
private:
    friend class ::ra_pod_scene50LA::Owner;
    struct RaSourceListMethod {
        OriginalNativeListMethodCustody50LA custody;
        //Missing source per-method selected18 service, NOT terrainSelected or
        //another registry. Slots remain unknown while actual count18 is0.
        std::unique_ptr<const OriginalWreckComponent50LA*[]> selected1C;
        std::uint32_t count18=0,growth20=0,capacity24=0;
        bool owned28Bit0=false,complete=false,published=false,typeLookupFailed=false,constructorAttempted=false,retired=false;
        std::optional<std::uint8_t> type2C,type2D;
    };
    struct RaSourceListDomain {
        const ::ra_pod_scene50LA::Owner* world=nullptr;
        std::array<RaSourceListMethod,20> methods; //Actual ten nodes, two distinct methods each.
    };
    std::unique_ptr<RaSourceListDomain> raSourceLists_;
    friend struct ::interactive_setup50DU::Context;
    friend class ::ra_source_camera50LA::OwnershipBridge;
public:
    //Per-racer query roles, the source's own 2129C0 order. Public: every
    //consumer names a row through row()/currentRow(), never a fixed ref.
    enum class RosterRole : std::uint8_t { main690,secondary364,terrain348,pod694,movement7A4 };
    static constexpr std::size_t rosterRoles=5,maximumRoster=8;
private:
    //Native immutable filter/list owner constructed at the actual per-racer
    //cold exclusion producer. Never a pointer into Bank-restored vector data.
    //F8 remains the separately authenticated mutable canonical Bank lane.
    struct SharedColdFilter {
        const std::uint32_t identity;
        FixedBuffer<std::uint32_t> first,second,categories;
        CollectionFilter50BX view{};
        explicit SharedColdFilter(const CollisionFilter48J& source):identity(source.identity),
            first(source.identityLists[0].identities.size()),
            second(source.identityLists[1].identities.size()),categories(source.categoryMasks.size()) {
            for(std::size_t i=0;i<first.size();++i)first[i]=source.identityLists[0].identities[i];
            for(std::size_t i=0;i<second.size();++i)second[i]=source.identityLists[1].identities[i];
            for(std::size_t i=0;i<categories.size();++i)categories[i]=source.categoryMasks[i];
            view.categoryA4=source.categoryA4;view.acceptMaskB8=source.acceptMaskB8;view.flags31=source.flags31;
            view.identities[0]={first.data(),first.size(),static_cast<std::int32_t>(first.size()),source.identityLists[0].mode};
            view.identities[1]={second.data(),second.size(),static_cast<std::int32_t>(second.size()),source.identityLists[1].mode};
            view.categories={categories.data(),categories.size(),static_cast<std::int32_t>(categories.size()),source.categoryModeD4};
        }
        bool matches(const CollisionFilter48J& source)const noexcept {
            if(identity!=source.identity||view.categoryA4!=source.categoryA4||
               view.acceptMaskB8!=source.acceptMaskB8||view.flags31!=source.flags31||
               view.categories.mode!=source.categoryModeD4)return false;
            const FixedBuffer<std::uint32_t>* words[]={&first,&second,&categories};
            const std::vector<std::uint32_t>* actual[]={&source.identityLists[0].identities,
                &source.identityLists[1].identities,&source.categoryMasks};
            for(unsigned i=0;i<3;++i){
                if(words[i]->size()!=actual[i]->size())return false;
                for(std::size_t n=0;n<words[i]->size();++n)if((*words[i])[n]!=(*actual[i])[n])return false;
            }
            return view.identities[0].mode==source.identityLists[0].mode&&
                view.identities[1].mode==source.identityLists[1].mode;
        }
    };
    struct SharedRosterObject {
        const native_racer_collision50HW::Owner* const collision;
        const std::size_t slot;
        const RosterRole role;
        const CollectionRef50BX reservedRef; //Capacity identity, never published E0.
        Membership membership;
        //Actual cold producer custody, not shared candidate publication. The
        //filter pointer is a logical Bank lane; never read another slot from it.
        CollisionFilter48J* canonicalFilter=nullptr;
        const Type2ResourceView49X* type2Resource=nullptr;
        const PodShapeSource49W* type0Resource=nullptr;
        const OriginalRacerMainBodyOwner50LA* bodyA0=nullptr;
        //Positive native348 linked-body custody from the actual cold source
        //setup. Not a guest348 Entity or auxiliary120/124/null publication.
        const OriginalRacerMainBodyOwner50LA* nativeTerrainBody128=nullptr;
        std::unique_ptr<live_terrain_probe50fr::Owner> terrainResource;
        std::unique_ptr<native_terrain_result50LA::Owner> terrainResult90;
        //Distinct unbanked replacement, never the counted source348 route.
        enum class NativeTerrainCut {none,admitted,removed,withdrawn,returned,attached,published,retired};
        NativeTerrainCut nativeTerrainCut=NativeTerrainCut::none;
        std::uint32_t nativeTerrainPC=0;
        std::unique_ptr<native_repulsor_payload50LA::Owner> nativeTerrainPayload;
        std::unique_ptr<const SharedColdFilter> coldFilter;
        std::optional<PodLiveQueryContext50IZ> podContext; //Actual saved per-slot binding; included in row budget.
        std::uint32_t identity=0;
        std::uint64_t generation=0,effectRevision=0; //Native actual owner writes only; not guest history.
        bool coldBound=false;
        SharedRosterObject(const native_racer_collision50HW::Owner& owner,std::size_t slot_,
            RosterRole role_,CollectionRef50BX ref,std::size_t capacity)
            :collision(&owner),slot(slot_),role(role_),reservedRef(ref),membership(capacity){}
    };
    struct SharedRosterAllocation {
        std::array<std::unique_ptr<SharedRosterObject>,maximumRoster*rosterRoles> objects;
        std::size_t count=0;
        std::size_t coldPayloadRemaining=0; //Actual unused fixed native budget, never widened.
        std::array<bool,maximumRoster> filterAttempted{}; //One cold filter construction per slot, never retried.
        bool retired=false;
    };
    //Legacy-layout 694/7A4 rows: the undertrack owner's two memberships, bound
    //once at cold before seal. Pointers only; the memberships stay Banked there.
    struct LegacyRecoveryRow { LiveZoneMembership50EN* state=nullptr; LiveZoneStorage50EN storage{}; };
    std::array<LegacyRecoveryRow,2> legacyRecovery_{};
    //Current racer's roster slot. Banked per racer in BOTH layouts, written
    //only by claimRosterSlot inside that slot's first cold visit, so inside any
    //Bank visit it names the visited slot and outside visits it is slot 0.
    std::size_t rosterSlot_=0;
    bool rosterSlotClaimed_=false;
public:
    // Stage B P1: the ONE place that defines where a racer's 690/364/348/694/7A4
    // query row lives. Legacy layout (PORT STAND-IN, Stage B switch: kept only
    // for RA's borrowed-world route and the no-roster/no-recovery route until
    // RA's shared route lands with the camera step): only the live racer copy
    // exists, so only rosterSlot_ is admitted. Shared layout (every other course):
    // that slot's own typed row. Every refusal returns an empty RowRef; nothing
    // is ever indexed out of range.
    struct RowRef {
        CollectionRef50BX ref=0;                //E0 / registration ref, <= count.objects
        LiveZoneMembership50EN* state=nullptr;  //the row's live membership
        LiveZoneStorage50EN storage{};          //its links/entries backing
        SharedRosterObject* row=nullptr;        //shared layout only
        explicit operator bool()const noexcept{return ref&&state;}
    };
    RowRef row(std::size_t slot,RosterRole role) noexcept {
        const auto r=static_cast<std::size_t>(role);
        if(r>=rosterRoles||slot>=maximumRoster)return {};
        if(sharedRoster_){
            const auto& a=*sharedRoster_;
            if(a.retired||slot>=a.count||a.count>maximumRoster)return {};
            auto* p=a.objects[slot*rosterRoles+r].get();
            if(!p||p->slot!=slot||p->role!=role||!p->reservedRef||p->reservedRef>count.objects)return {};
            return {p->reservedRef,&p->membership.state,p->membership.storage(),p};
        }
        if(slot!=rosterSlot_)return {};
        const auto fixed=[&](CollectionRef50BX ref,Membership& m)->RowRef {
            if(!ref||ref>count.objects)return {};
            return {ref,&m.state,m.storage(),nullptr};
        };
        switch(role){
        case RosterRole::main690:return fixed(count.legacyMainQuery,legacyMainMembership);
        case RosterRole::secondary364:return fixed(count.legacySecondaryQuery,legacySecondaryMembership);
        case RosterRole::terrain348:return fixed(count.legacyTerrainQuery,legacyTerrainMembership);
        case RosterRole::pod694:case RosterRole::movement7A4:{
            const auto ref=role==RosterRole::pod694?count.legacyPodQuery:count.legacyMovementQuery;
            const auto& b=legacyRecovery_[r-3u];
            if(!ref||ref>count.objects||!b.state)return {};
            return {ref,b.state,b.storage,nullptr};}
        }
        return {};
    }
    RowRef currentRow(RosterRole role) noexcept{return row(rosterSlot_,role);}
    //A row's collector object carries that row's OWN immutable cold list
    //backing (shared world), never the banked live filter vectors, which hold
    //whichever racer is visited. Legacy rows (no typed row): unchanged.
    //Refused (false) for a shared row whose cold filter was never built.
    static bool useRowFilter(const RowRef& r,CollectionObject50BX& object) noexcept {
        if(!r.row)return true;
        if(!r.row->coldFilter)return false;
        object.filter=r.row->coldFilter->view;return true;
    }
    std::size_t rosterSlot()const noexcept{return rosterSlot_;}
    //Every row that exists now: legacy, the live racer's reserved rows (694/7A4
    //only when recovery reserved and lent them); shared, all count*5 rows. A
    //row that exists but cannot be resolved stops the walk with false.
    template<class F> bool forEachPresentRow(F&& f) noexcept {
        if(sharedRoster_){
            if(sharedRoster_->retired||sharedRoster_->count>maximumRoster)return false;
            for(std::size_t slot=0;slot<sharedRoster_->count;++slot)
                for(std::size_t r=0;r<rosterRoles;++r){
                    const auto role=static_cast<RosterRole>(r);auto x=row(slot,role);
                    if(!x||!f(slot,role,x))return false;
                }
            return true;
        }
        const CollectionRef50BX reserved[rosterRoles]={count.legacyMainQuery,count.legacySecondaryQuery,
            count.legacyTerrainQuery,count.legacyPodQuery,count.legacyMovementQuery};
        for(std::size_t r=0;r<rosterRoles;++r){
            //Not reserved, or 694/7A4 never lent (recovery owner not prepared): no row exists.
            if(!reserved[r]||(r>=3u&&!legacyRecovery_[r-3u].state))continue;
            const auto role=static_cast<RosterRole>(r);auto x=row(rosterSlot_,role);
            if(!x||!f(rosterSlot_,role,x))return false;
        }
        return true;
    }
    //Every racer query row's membership is detached (idle/retirement checks
    //outside visits). Shared: all count*5 rows. Legacy: the three fixed rows
    //of the live copy, exactly as the course world owners checked before.
    bool rowsDetached()const noexcept {
        if(sharedRoster_){
            for(const auto& p:sharedRoster_->objects)
                if(p&&(p->membership.state.attached||p->membership.state.object))return false;
            return true;
        }
        for(const auto* m:{&legacyMainMembership,&legacySecondaryMembership,&legacyTerrainMembership})
            if(m->state.attached||m->state.object)return false;
        return true;
    }
    //Remove every racer query row's membership from the one world (teardown
    //after the Bank closed). Same row set as rowsDetached. False on the first
    //incomplete removal; earlier removals stay committed (real prefix).
    bool detachAllRows() noexcept {
        auto remove=[&](LiveZoneMembership50EN& m){
            return removeLiveZoneMembership50EN(lifecycle,world,m,membershipWork).status==LiveZoneStatus50EN::Complete&&
                !m.attached&&!m.object;};
        if(sharedRoster_){
            for(auto& p:sharedRoster_->objects)if(p&&!remove(p->membership.state))return false;
            return true;
        }
        for(auto* m:{&legacyMainMembership,&legacySecondaryMembership,&legacyTerrainMembership})
            if(!remove(m->state))return false;
        return true;
    }
    //Non-camera active collector extent (static owners plus every query row),
    //or the full extent including the trailing camera refs.
    std::size_t activeExtent(bool cameraDomain)const noexcept {
        if(cameraDomain)return count.objects;
        if(sharedRoster_)return count.terrain+rosterRoles*sharedRoster_->count;
        return count.terrain+runtimeDynamicReferenceReserve+count.legacyRecoveryReferences;
    }
    //Cold, before seal, legacy layout only: lend the undertrack owner's two
    //memberships as rows 3/4. Idempotent for the same pair; any other refused.
    bool bindLegacyRecoveryRows(LiveZoneMembership50EN& podMember,LiveZoneStorage50EN podStorage,
        LiveZoneMembership50EN& movementMember,LiveZoneStorage50EN movementStorage) noexcept {
        if(sharedRoster_||&podMember==&movementMember||!count.legacyPodQuery||!count.legacyMovementQuery)return false;
        const LegacyRecoveryRow next[2]={{&podMember,podStorage},{&movementMember,movementStorage}};
        for(const auto& n:next)
            if(!n.storage.links||!n.storage.entries||n.storage.capacity!=count.membership||
               static_cast<const void*>(n.storage.links)==static_cast<const void*>(n.storage.entries))return false;
        for(unsigned i=0;i<2;++i)if(legacyRecovery_[i].state&&(legacyRecovery_[i].state!=next[i].state||
            legacyRecovery_[i].storage.links!=next[i].storage.links||
            legacyRecovery_[i].storage.entries!=next[i].storage.entries||
            legacyRecovery_[i].storage.capacity!=next[i].storage.capacity))return false;
        legacyRecovery_[0]=next[0];legacyRecovery_[1]=next[1];return true;
    }
    //Inside that slot's own first cold Bank visit only (the caller proves the
    //visit). Writes only the banked lane; a second, different claim is refused.
    bool claimRosterSlot(std::size_t slot) noexcept {
        if(slot>=maximumRoster||(sharedRoster_&&slot>=sharedRoster_->count))return false;
        if(rosterSlotClaimed_)return rosterSlot_==slot;
        rosterSlot_=slot;rosterSlotClaimed_=true;return true;
    }
    // ---- Stage S-2 item 1: the per-slot row producers. Storage is the one
    // owner of the roster layout, so the cold producers that fill a slot's five
    // rows live here; every course's cold roster and the RA bridge call these
    // same functions. The CALLER proves it is inside that slot's own cold Bank
    // visit. Legacy layout (no rows): no effect, true. Every write below goes
    // through row(), which refuses a slot/role outside the allocation.
    struct RowColdInputs {
        const native_racer_collision50HW::Owner* collision=nullptr;
        std::array<std::uint32_t,rosterRoles> identities{};     //role order 690,364,348,694,7A4
        std::array<CollisionFilter48J*,rosterRoles> filters{};  //the racer's canonical filters
        const Type2ResourceView49X* mainResource=nullptr;
        const Type2ResourceView49X* secondaryResource=nullptr;
        const PodShapeSource49W* podShape=nullptr;              //694 type0 source
        const OriginalRacerMainBodyOwner50LA* podBody=nullptr;  //694 bodyA0 and 348+128 (racer340, 211F6C)
        const VehicleFrameState49W* frame=nullptr;              //the visited racer's frame (body numeric owner)
        const PodShapeSource49W* authoredShape=nullptr;         //the racer's authored parameters.shape
        std::uint64_t sessionGeneration=0;                      //caller's race session, compared, never stored
        const TerrainPrepareState48M* authoredTerrain=nullptr;  //the authored348 the row resource came from
        std::uint8_t collisionTypeTag=0;                        //the racer's query type tag (all five rows)
        std::uint8_t movementShapeTag=3;                        //7A4 type3 tag (11C710)
    };
    //The ONE admission of a racer340 body as the 694 bodyA0 (every course;
    //RA's stageNativePod694 calls it too). Read only. sessionGeneration 0
    //means the caller has no separate session stamp (RA staging: port
    //stand-in until RA's cold roster passes its aggregate generation).
    static bool podBodyAdmissible(const OriginalRacerMainBodyOwner50LA& body,const VehicleFrameState49W& frame,
        const PodShapeSource49W& shape,const PodShapeSource49W& authored,std::uint64_t sessionGeneration) noexcept {
        const auto& entity=body.entity();const auto* scene=body.scene288();
        if(body.canonicalNumericAddress()!=&frame.bodies.main||!scene||!scene->constructed||scene->released||
           !entity.reference00||!entity.objectKey||entity.kind!=OriginalWreckComponentKind50LA::body167920||
           !entity.sessionGeneration||scene->entity().sessionGeneration!=entity.sessionGeneration||
           (sessionGeneration&&entity.sessionGeneration!=sessionGeneration))return false;
        return !std::memcmp(shape.local40.v,authored.local40.v,sizeof(shape.local40.v))&&
            !std::memcmp(shape.rootCenter.v,authored.rootCenter.v,sizeof(shape.rootCenter.v))&&
            !std::memcmp(shape.rootHalf.v,authored.rootHalf.v,sizeof(shape.rootHalf.v));
    }
    bool sharedLayout()const noexcept{return bool(sharedRoster_);}
    //True while any row still borrows this body (teardown preflight).
    bool rowsHoldBody(const OriginalRacerMainBodyOwner50LA* body)const noexcept {
        if(!body||!sharedRoster_)return false;
        for(const auto& p:sharedRoster_->objects)
            if(p&&(p->bodyA0==body||p->nativeTerrainBody128==body))return true;
        return false;
    }
    //11C5B4..11C6E4: the slot's 348 resource and its separate result90, built
    //once from the still-live authored segment, before any cold binding.
    bool constructRowTerrain(std::size_t slot,const native_racer_collision50HW::Owner& collision,
        const LocalSegment48I& segment) noexcept {
        if(!sharedRoster_)return true;
        auto x=row(slot,RosterRole::terrain348);
        if(!x.row)return false;
        auto& r=*x.row;
        if(r.collision!=&collision||r.coldBound||!r.terrainResource||r.terrainResource->initialized||
           !r.terrainResult90||r.terrainResult90->constructed_||r.terrainResult90->revoked_)return false;
        if(!live_terrain_probe50fr::initialize(*r.terrainResource,segment))return false;
        return r.terrainResult90->construct();
    }
    //2129C0 per-racer constructor cut: identities, canonical filter lanes,
    //immutable cold filter copies (payload reserved before allocation, one
    //attempt per slot) and resource custody for all five rows of one slot.
    bool bindRowColdOwners(std::size_t slot,const RowColdInputs& in) noexcept {
        if(!sharedRoster_)return true;
        auto& a=*sharedRoster_;
        if(a.retired||slot>=a.count||slot>=a.filterAttempted.size()||a.filterAttempted[slot]||
           !in.collision||!in.mainResource||!in.secondaryResource||!in.podShape||!in.podBody||
           !in.frame||!in.authoredShape||!in.authoredTerrain||
           !podBodyAdmissible(*in.podBody,*in.frame,*in.podShape,*in.authoredShape,in.sessionGeneration))return false;
        //Derived from the admitted body, never supplied by the caller.
        const std::uint64_t generation=in.podBody->entity().sessionGeneration;
        std::array<SharedRosterObject*,rosterRoles> rows{};
        for(std::size_t i=0;i<rosterRoles;++i){
            auto* p=row(slot,static_cast<RosterRole>(i)).row;
            if(!p||p->collision!=in.collision||p->coldBound||p->identity||p->coldFilter||
               p->membership.state.attached||p->membership.state.object||
               !in.identities[i]||!in.filters[i]||in.filters[i]->identity!=in.identities[i])return false;
            for(std::size_t j=0;j<i;++j)if(in.identities[i]==in.identities[j])return false;
            rows[i]=p;
        }
        auto& terrain=*rows[static_cast<std::size_t>(RosterRole::terrain348)];
        if(!terrain.terrainResource||!terrain.terrainResource->initialized||
           !terrain.terrainResult90||!terrain.terrainResult90->constructed_||terrain.terrainResult90->revoked_||
           !live_terrain_probe50fr::matches(*terrain.terrainResource,*in.authoredTerrain))return false;
        //212918..21299C reciprocal first-list exclusions, already written by
        //the caller's visit; checked here against this slot's own identities.
        const auto& id=in.identities;
        const std::uint32_t expected0[]={id[1],id[3],id[4],id[2]};
        const std::uint32_t expected1[]={id[0],id[3],id[4]};
        const std::uint32_t expected3[]={id[0],id[4],id[2]};
        const std::uint32_t* expected[]={expected0,expected1,nullptr,expected3,nullptr};
        constexpr std::size_t sizes[]={4,3,0,3,0};
        //Port stand-in (writeRosterPodExclusionStandIn): 690/364/7A4 may carry
        //the fixed other-pod694 exclusion list; 348/694 never do.
        constexpr bool standIn[]={true,true,false,false,true};
        std::size_t payload=0;
        for(std::size_t i=0;i<rosterRoles;++i){
            const auto& f=*in.filters[i];const auto& first=f.identityLists[0];const auto& second=f.identityLists[1];
            const bool secondOk=second.identities.empty()||
                (standIn[i]&&second.identities.size()==maximumRoster);
            if(first.identities.size()!=sizes[i]||first.mode!=1||second.mode!=1||!secondOk)return false;
            for(std::size_t n=0;n<sizes[i];++n)if(first.identities[n]!=expected[i][n])return false;
            std::size_t bytes=sizeof(SharedColdFilter);
            for(const auto n:{first.identities.size(),second.identities.size(),f.categoryMasks.size()}){
                if(n>static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max())||
                   n>(std::numeric_limits<std::size_t>::max()-bytes)/sizeof(std::uint32_t))return false;
                bytes+=n*sizeof(std::uint32_t);
            }
            if(bytes>a.coldPayloadRemaining||payload>a.coldPayloadRemaining-bytes)return false;
            payload+=bytes;
        }
        //Reserve the exact payload first; a partial construction keeps its
        //reservation and its terminal attempt, never retries or duplicates.
        a.filterAttempted[slot]=true;a.coldPayloadRemaining-=payload;
        try {
            for(std::size_t i=0;i<rosterRoles;++i)
                rows[i]->coldFilter=std::make_unique<const SharedColdFilter>(*in.filters[i]);
        }catch(...){return false;}
        for(std::size_t i=0;i<rosterRoles;++i){
            auto& r=*rows[i];r.identity=id[i];r.canonicalFilter=in.filters[i];
            r.generation=generation;r.coldBound=true;
        }
        rows[0]->type2Resource=in.mainResource;rows[1]->type2Resource=in.secondaryResource;
        terrain.type2Resource=&terrain.terrainResource->resource;
        //21207C/1B11FC binds348+128 to this genuine main body owner.
        terrain.nativeTerrainBody128=in.podBody;
        auto& pod=*rows[static_cast<std::size_t>(RosterRole::pod694)];
        pod.type0Resource=in.podShape;pod.bodyA0=in.podBody;
        //7A4 endpoints/metadata belong to a later actual recovery producer.
        //2129C0 constructs every racer's query objects at load: publish each
        //row's collector record (identity, own cold list backing, filter lanes)
        //now, so the one world never holds an identity-less object. Bounds stay
        //empty until the row's own visit attaches and projects it; an unattached
        //row is in no zone list and is never collected.
        constexpr std::uint8_t shapeTags[rosterRoles]={2,2,2,0,0};
        for(std::size_t i=0;i<rosterRoles;++i){
            auto& r=*rows[i];
            const auto shape=i==static_cast<std::size_t>(RosterRole::movement7A4)?in.movementShapeTag:shapeTags[i];
            auto projected=projectCollectionObject50CF(*in.filters[i],CollisionSphere48J{},in.collisionTypeTag,shape,0u);
            if(!projected.valid||projected.object.identity!=r.identity||
               !r.reservedRef||r.reservedRef>count.objects)return false;
            projected.object.filter=r.coldFilter->view;
            objects[r.reservedRef-1u]=projected.object;source[r.reservedRef-1u]=projected.state;
            ++r.effectRevision;
        }
        return true;
    }
    //Withdraw a slot's cold custody (resource borrows); memberships and the
    //immutable resource backing stay until the World retires.
    void revokeRowColdOwners(std::size_t slot) noexcept {
        if(!sharedRoster_)return;
        for(std::size_t i=0;i<rosterRoles;++i){
            auto* p=row(slot,static_cast<RosterRole>(i)).row;
            if(!p)continue;
            p->canonicalFilter=nullptr;p->bodyA0=nullptr;
            p->nativeTerrainBody128=nullptr;p->coldBound=false;
            if(p->terrainResult90)p->terrainResult90->revoke();
        }
    }
    //The slot's saved 694 binding, emplaced once in its own row (charged to
    //the row budget). Returns the stable context, or null on refusal.
    const PodLiveQueryContext50IZ* bindRowPodContext(std::size_t slot,const PodShapeSource49W& shape) noexcept {
        if(!sharedRoster_)return nullptr;
        auto* pod=row(slot,RosterRole::pod694).row;auto* movement=row(slot,RosterRole::movement7A4).row;
        if(!pod||!movement||!pod->coldBound||pod->podContext||
           pod->membership.state.attached||movement->membership.state.attached)return nullptr;
        pod->podContext.emplace(PodLiveQueryContext50IZ{{lifecycle,world,pod->membership.state,
            membershipWork,pod->reservedRef},&shape});
        return &*pod->podContext;
    }
    //Grid publication of one attached 690/364/348 row into the collector
    //backing at its own reserved ref, with the row's own cold list backing.
    bool publishRowGridRole(std::size_t slot,RosterRole role,const LiveZoneBounds50EN& bounds,
        std::uint8_t collisionTypeTag) noexcept {
        if(role>RosterRole::terrain348)return false;
        auto* p=row(slot,role).row;if(!p)return false;
        auto& r=*p;const auto& member=r.membership.state;
        if(!r.coldBound||!r.coldFilter||!r.canonicalFilter||!r.coldFilter->matches(*r.canonicalFilter)||
           !r.reservedRef||r.reservedRef>count.objects||!member.attached||member.object!=r.reservedRef||
           member.lastShapeTag!=2u||member.lastType2Resource!=r.type2Resource||
           bounds.type2Resource!=r.type2Resource||objects[r.reservedRef-1u].identity||
           r.effectRevision==std::numeric_limits<std::uint64_t>::max())return false;
        CollisionSphere48J sphere{};std::memcpy(sphere.center.v,bounds.bounds.cachedCenter.v,12);
        sphere.radius=bounds.bounds.cachedRadius;
        auto projected=projectCollectionObject50CF(*r.canonicalFilter,sphere,collisionTypeTag,2u,
            std::bit_cast<std::uint32_t>(bounds.bounds.cachedCenter.v[3]));
        if(!projected.valid||projected.object.identity!=r.identity)return false;
        projected.object.filter=r.coldFilter->view; //Own cold list backing, never Bank-restored vectors.
        objects[r.reservedRef-1u]=projected.object;source[r.reservedRef-1u]=projected.state;
        ++r.effectRevision;return true;
    }
    //Grid retirement of a slot's 690/364/348 rows. Null on success, else the
    //refusal; earlier successful removals stay committed (real prefix).
    const char* retireRowGridRoles(std::size_t slot) noexcept {
        if(!sharedRoster_)return nullptr;
        for(std::size_t i=0;i<3u;++i){
            auto* p=row(slot,static_cast<RosterRole>(i)).row;
            if(!p)return "shared grid retirement row absent";
            auto& r=*p;auto& object=objects[r.reservedRef-1u];
            if((object.identity&&object.identity!=r.identity)||
               (r.membership.state.attached&&r.membership.state.object!=r.reservedRef)||
               r.effectRevision==std::numeric_limits<std::uint64_t>::max())
                return "shared grid removal identity/revision differs";
            if(removeLiveZoneMembership50EN(lifecycle,world,r.membership.state,membershipWork).status!=
                LiveZoneStatus50EN::Complete)return "actual shared grid removal incomplete; prefix retained";
            if(object.identity){object={};source[r.reservedRef-1u]={};++r.effectRevision;}
        }
        return nullptr;
    }
private:
    //Actual span inventory for fresh shared admission. The caller tests these
    //addresses against its REAL registered Bank spans before lending a pin.
    template<class Check> bool sharedSpansAbsent(Check&& absent)const noexcept {
        if(!sharedRoster_||sharedRoster_->retired)return false;
        const auto array=[&](const auto& a){return absent(a.data(),a.size()*sizeof(*a.data()));};
        if(!absent(&lifecycle,sizeof(lifecycle))||!array(registries)||!array(registrations)||
           !array(blocks)||!array(trees)||!array(zones)||!absent(&queue,sizeof(queue))||!array(events)||
           !absent(world.portalHeads,world.zoneCount*sizeof(*world.portalHeads))||
           !absent(world.portalLinks,world.portalLinkCount*sizeof(*world.portalLinks))||
           !absent(world.portals,world.portalCount*sizeof(*world.portals))||
           !array(objects)||!array(source)||!array(treeSource))return false;
        const auto membership=[&](const Membership& m){return absent(&m.state,sizeof(m.state))&&
            absent(m.links.data(),m.links.size()*sizeof(*m.links.data()))&&
            absent(m.entries.data(),m.entries.size()*sizeof(*m.entries.data()));};
        for(const auto* m:{&legacyMainMembership,&legacySecondaryMembership,&legacyTerrainMembership})
            if(!membership(*m))return false;
        for(std::size_t i=0;i<movement.size();++i){const auto& c=movement[i].collision;
            if(!absent(&c.filter.flagsF8,sizeof(c.filter.flagsF8))||
               !absent(&c.copied40,sizeof(c.copied40))||!absent(&c.copied50,sizeof(c.copied50)))return false;
        }
        for(const auto& row:sharedRoster_->objects)if(row){
            if(!absent(row.get(),sizeof(*row))||!membership(row->membership)||
               (row->terrainResource&&!absent(row->terrainResource.get(),sizeof(*row->terrainResource)))||
               (row->terrainResult90&&!absent(row->terrainResult90.get(),sizeof(*row->terrainResult90))))return false;
            if(row->coldFilter&&(!absent(row->coldFilter.get(),sizeof(*row->coldFilter))||
               !array(row->coldFilter->first)||!array(row->coldFilter->second)||!array(row->coldFilter->categories)))return false;
        }
        return true;
    }
    bool sharedRosterCanRetire()const noexcept {
        if(!sharedRoster_)return true;
        if(queue.count||(collectionInput.objectCount!=count.terrain&&collectionInput.objectCount!=activeExtent(false))||collectionInput.query||
           collectionInput.zoneHead||collectionInput.initialActiveTree)return false;
        // Stage B has no query/Follow/reset borrow API. Refuse all actually
        // published row state rather than assuming allocated means detached.
        for(const auto& p:sharedRoster_->objects)if(p){
            if(p->coldBound||p->canonicalFilter||p->bodyA0||
               p->membership.state.attached||p->membership.state.object||
               !p->reservedRef||p->reservedRef>count.objects)return false;
            const auto index=p->reservedRef-1u;
            const auto& r=registrations[index];
            if(r.owner||r.slot||r.encodedObject||objects[index].identity||
               source[index].epoch||source[index].flagsF8)return false;
            for(const auto& e:p->membership.entries)
                if(e.used||e.registration.owner||e.registration.slot||e.registration.encodedObject)return false;
        }
        return true;
    }
    bool retireSharedRosterAllocation()noexcept {
        if(!sharedRoster_)return true;
        if(!sharedRosterCanRetire())return false;
        // No fabricated removal: every row must still be truly unpublished.
        for(auto& p:sharedRoster_->objects)p.reset();
        sharedRoster_->retired=true;return true;
    }

    // Declared before the byte reference and every query field. All escaped
    // query-byte borrows are protected by this actual process-owner retain.
    const std::shared_ptr<OriginalApplicationEntities50LA> application_;
    std::shared_ptr<native_spring_detector_policy50LA::Owner> nativeDetectorPolicy_;
    static std::shared_ptr<OriginalApplicationEntities50LA> requireApplication(
        std::shared_ptr<OriginalApplicationEntities50LA> app) {
        if(!app||!app->application10())throw std::invalid_argument("actual live Application collision authority required");
        return app;
    }
public:
    // Keep borrowed source backing and the exclusive
    // serial world lease alive until every registry/scratch member is retired.
    const std::shared_ptr<void> borrowedWorldLease;
private:
    //Destroy row backing before the exclusive actual World lease.
    std::unique_ptr<SharedRosterAllocation> sharedRoster_;
public:
    const Counts count;
    FixedBuffer<Type2StaticCandidate49X*> candidates;
    FixedBuffer<std::uint32_t> candidateZones;
    FixedBuffer<ZoneMethod50CL> candidateMethods;
    FixedBuffer<ContactMaterial50HI> contactMaterials;
    FixedBuffer<CandidateHandle50CB> handles;
    FixedBuffer<Type3Candidate48P> movement,registeredMovement;
    FixedBuffer<TerrainCandidate48J*> terrainTargets;
    FixedBuffer<TerrainCandidate48J> terrainSelected;
    FixedBuffer<TerrainCandidateEffects48J> terrainScratchEffects,terrainEffects;
    FixedBuffer<std::size_t> registeredIndices,probeIndices,resolveScratch;
    std::size_t registeredCount=0,probeCount=0;

    SearchStorage50BS movementRegistry{};
    SearchListBlock50BR movementBlocks[2]{};
    SearchListInput50BR movementView{};
    SearchListCursor50BR movementCursor{};
    FixedBuffer<SearchStorage50BS> registries,membershipScratchRegistries;
    FixedBuffer<ZoneRegistration50CJ> registrations;
    FixedBuffer<SearchListBlock50BR> blocks;
    FixedBuffer<CollectionTree50BX> trees;
    FixedBuffer<CollectionZone50BX> zones;
    ZoneLifecycle50CL lifecycle{};
    // SAME process3E0730 byte across serial worlds and all existing query
    // consumers. Reference preserves existing address-taking and write sites.
    std::uint8_t& globalValid3E0730;
    ZoneRegistrationScratch50CJ registrationScratch{};
    FixedBuffer<ZoneRegistrationCommand50CQ> events;
    ZoneRegistrationQueue50CQ queue{};

    FixedBuffer<LiveZonePortalInput50EN> authoredPortals;
    FixedBuffer<CollectionRef50BX> portalHeads;
    FixedBuffer<LiveZonePortalLink50EN> portalLinks,portalScratchLinks;
    FixedBuffer<PortalGeometry50EO> portalGeometry;
    LiveZoneWorld50EN world{};
    Membership legacyMainMembership,legacySecondaryMembership,legacyTerrainMembership;
    FixedBuffer<CollectionZoneLink50BX> membershipScratchLinks;
    FixedBuffer<LiveZoneEntry50EN> membershipScratchEntries;
    LiveZoneWorkspace50EN membershipWork{};

    FixedBuffer<CollectionObject50BX> objects;
    FixedBuffer<CollectionObjectState50BX> source,scratch,first,second;
    FixedBuffer<CollectionTreeState50BX> treeSource,treeScratch,treeFirst,treeSecond;
    FixedBuffer<std::uint32_t> scratchIds,firstIds,secondIds,mergeScratch,mergedIds;
    CandidateCollectionInput50BX collectionInput{};

private:
    friend class Owner;
    // Only Owner's private authenticated borrowed route can select a lease.
    explicit Storage(Counts n,const LiveZoneWorld50EN* borrowed,
        std::shared_ptr<void> lease,std::shared_ptr<OriginalApplicationEntities50LA> app,bool sharedFresh=false)
        :application_(requireApplication(std::move(app))),borrowedWorldLease(std::move(lease)),count(n),candidates(n.terrain),candidateZones(n.terrain),candidateMethods(n.terrain),
        contactMaterials(n.terrain),handles(n.terrain),movement(n.terrain),
        registeredMovement(n.terrain),terrainTargets(n.terrain),terrainSelected(n.selected),
        terrainScratchEffects(n.selected),terrainEffects(n.selected),
        registeredIndices(n.selected),probeIndices(n.selected),resolveScratch(sourceSelectionCapacity),
        registries(n.methods),membershipScratchRegistries(n.methods),registrations(n.objects),
        blocks(n.blocks),trees(n.methods),zones(n.zones),
        globalValid3E0730(application_->collisionGlobalValid3E0730_),events(n.events),
        authoredPortals(borrowed?0:n.portals),portalHeads(borrowed?0:n.zones),portalLinks(borrowed?0:n.portalLinks),
        portalScratchLinks(n.portalLinks),portalGeometry(borrowed?0:n.portals),
        //Legacy-layout fixed memberships (port stand-in: RA and the no-roster route only);
        //zero backing on the shared layout, where every row owns its own.
        legacyMainMembership(sharedFresh?0:n.membership),legacySecondaryMembership(sharedFresh?0:n.membership),
        legacyTerrainMembership(sharedFresh?0:n.membership),
        membershipScratchLinks(n.membership),membershipScratchEntries(n.membership),
        objects(n.objects),source(n.objects),scratch(n.objects),first(n.objects),second(n.objects),
        treeSource(n.methods),treeScratch(n.methods),treeFirst(n.methods),treeSecond(n.methods),
        scratchIds(sourceSelectionCapacity),firstIds(sourceSelectionCapacity),secondIds(sourceSelectionCapacity),
        mergeScratch(sourceSelectionCapacity),mergedIds(sourceSelectionCapacity){
        // This facade points at the same cache/topology owner used for RA's
        // two navigation bindings. It never creates a second cache owner.
        if(n.sharedRosterCount&&!sharedFresh)throw std::invalid_argument("shared roster allocation requires private fresh factory");
        if(borrowed)world=*borrowed;
    }
public:
    explicit Storage(Counts n,std::shared_ptr<OriginalApplicationEntities50LA> app)
        :Storage(n,nullptr,{},std::move(app)){}
    ~Storage()=default; //Bridge gates any published lifetime before releasing Storage.
    Storage(const Storage&)=delete;
    Storage& operator=(const Storage&)=delete;
    Storage(Storage&&)=delete;
    Storage& operator=(Storage&&)=delete;

    CandidateCollectionBuffers50BX collectionBuffer(CollectionObjectState50BX* o,
        CollectionTreeState50BX* t,std::uint32_t* ids)const noexcept {
        CandidateCollectionBuffers50BX b{};b.objects=o;b.objectCapacity=count.objects;
        b.trees=t;b.treeCapacity=count.methods;b.identities=ids;
        b.identityCapacity=sourceSelectionCapacity;return b;
    }
    CandidateCollectionState50BX initialCollection()const noexcept {
        CandidateCollectionState50BX s{};s.objects=source.data();s.objectCapacity=count.objects;
        s.trees=treeSource.data();s.treeCapacity=count.methods;return s;
    }
    VehicleCandidateWorkspace50CA collectionWorkspace() noexcept {
        VehicleCandidateWorkspace50CA w{};
        w.collector.buffers=collectionBuffer(scratch.data(),treeScratch.data(),scratchIds.data());
        w.first=collectionBuffer(first.data(),treeFirst.data(),firstIds.data());
        w.second=collectionBuffer(second.data(),treeSecond.data(),secondIds.data());
        w.mergeScratch={mergeScratch.data(),mergeScratch.size()};
        w.merged={mergedIds.data(),mergedIds.size()};return w;
    }
    const native_spring_detector_policy50LA::Owner* nativeDetectorPolicy()const noexcept {
        return nativeDetectorPolicy_.get();
    }
    // Mirrors the current late movement registry iteration without assigning
    // vector-owning candidate objects (which could allocate after cold setup).
    bool gatherMovement() noexcept {
        if(prepareSearchView50BT(movementRegistry,movementBlocks,2,movementView)!=
            SearchStorageStatus50BS::Complete)return false;
        resetSearchList50BR(movementCursor,true);registeredCount=0;
        for(;;){
            std::uint32_t id=0;
            if(nextSearchList50BR(movementView,movementCursor,id)!=SearchListStatus50BR::Complete)return false;
            if(!id)return true;
            if((id&3u)||id/4u<1u||id/4u>count.terrain||registeredCount>=count.terrain)return false;
            const std::size_t index=id/4u-1u;
            const auto& input=movement[index];auto& output=registeredMovement[registeredCount];
            if(!live_terrain_probe50fr::detail::copyCandidate(input.collision,output.collision))return false;
            output.shapeType=input.shapeType;output.compound=input.compound;output.hasMaterialScene=input.hasMaterialScene;
            output.sceneName=input.sceneName;output.sceneNameBytes=input.sceneNameBytes;
            registeredIndices[registeredCount++]=index;
        }
    }
    // Replace the corresponding old world-storage registration block in
    // native_racer_owners50hn.inc. Do not register this Storage/FixedBuffer or
    // any std::vector object itself. External candidate owner lanes and the
    // actual vehicle/query state remain the caller's existing registrations.
    template<class Bank> void registerMutableOwners(Bank& bank) {
        if(nativeDetectorPolicy_)nativeDetectorPolicy_->registerMutableOwners(bank);
        auto elements=[&](auto& a){bank.elements(a.data(),a.size());};
        if(sharedRoster_){
            if(sharedRoster_->retired)throw std::runtime_error("retired shared roster storage");
            // Actual source descriptors/cursors below are per-invocation or
            // per-racer outputs. Shared topology, method state, object effects,
            // static movement mirrors and all memberships MUST NOT be banked.
            // collectionInput is the one world's collector descriptor: its
            // extent (static prefix, every racer row, then the camera tail once
            // published) is the same for every racer. Each collection writes
            // the visited racer's own query ref, zone links, bounds and
            // resource before collecting, so nothing of it is banked.
            bank.owner(rosterSlot_);bank.owner(rosterSlotClaimed_);
            elements(registeredIndices);bank.owner(registeredCount);
            elements(probeIndices);bank.owner(probeCount);
            return;
        }
        for(auto& c:movement){bank.owner(c.collision.filter.flagsF8);
            bank.owner(c.collision.copied40);bank.owner(c.collision.copied50);}
        bank.owner(lifecycle);elements(registries);elements(registrations);
        elements(blocks);elements(trees);elements(zones);bank.owner(queue);elements(events);
        // Bank the selected real backing, including the borrowed-world route.
        bank.elements(world.portalHeads,world.zoneCount);
        bank.elements(world.portalLinks,world.portalLinkCount);
        bank.elements(world.portals,world.portalCount);
        for(auto* m:{&legacyMainMembership,&legacySecondaryMembership,&legacyTerrainMembership}){
            bank.owner(m->state);elements(m->links);elements(m->entries);}
        elements(objects);elements(source);elements(treeSource);bank.owner(collectionInput);
        bank.owner(rosterSlot_);bank.owner(rosterSlotClaimed_);
        elements(registeredIndices);bank.owner(registeredCount);elements(probeIndices);bank.owner(probeCount);
    }
};
using RosterRole=Storage::RosterRole;
using RosterRowRef=Storage::RowRef;

// PORT STAND-IN (banking.md step S; retired with the original in-query
// pod-vs-pod response, step R). Once every racer's rows live in one shared
// world, each racer's 690 and 364 would collect the other racers' 694 rows
// (target bit, pod category). Legacy courses have no in-query pod producer
// (they would refuse) and RA's would double the host contact join. So every
// racer's 690/364/7A4 query excludes ALL roster racers' 694 identities through
// identityLists[1] (mode 1 = exclude): the collector returns no other pod and
// the host join stays the only pod-vs-pod response, exactly as today.
// 348 needs no entry: its category list admits floor targets only, never a pod
// (and its namespace checks require empty lists). This is the ONE place that
// defines the list: maximumRoster words, slot i's 694 at index i, 0 = unused.
// The list is the same for every racer, so it is NOT banked: sized here
// before the Bank seals, then written once by writeRosterPodExclusionStandIn.
inline bool sizeRosterPodExclusionStandIn(CollisionFilter48J& f) {
    auto& list=f.identityLists[1];
    if(list.mode!=1||!list.identities.empty())return false;
    list.identities.assign(Storage::maximumRoster,0u);return true;
}
inline bool rosterPodExclusionStandInSized(const CollisionFilter48J& f) noexcept {
    return f.identityLists[1].mode==1&&f.identityLists[1].identities.size()==Storage::maximumRoster;
}
//Refused (nothing written) unless sized, every id nonzero and distinct, and
//count within the roster ceiling. Writes only inside the fixed backing.
inline bool writeRosterPodExclusionStandIn(CollisionFilter48J& f,const std::uint32_t* pods,
    std::size_t count) noexcept {
    if(!rosterPodExclusionStandInSized(f)||!pods||!count||count>Storage::maximumRoster)return false;
    for(std::size_t i=0;i<count;++i){
        if(!pods[i])return false;
        for(std::size_t j=0;j<i;++j)if(pods[j]==pods[i])return false;
    }
    auto& words=f.identityLists[1].identities;
    for(std::size_t i=0;i<Storage::maximumRoster;++i)words[i]=i<count?pods[i]:0u;
    return true;
}

namespace detail {
inline void need(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
struct Budget {
    std::size_t capacity=0,used=0;
    template<class T> void add(std::size_t count,std::size_t repeats=1){
        need(!repeats||count<=std::numeric_limits<std::size_t>::max()/repeats,"track storage extent overflow");
        count*=repeats;
        need(count<=std::numeric_limits<std::size_t>::max()/sizeof(T),"track storage byte overflow");
        const auto bytes=count*sizeof(T);
        need(used<=capacity&&bytes<=capacity-used,"track storage requested payload exceeds cold budget");used+=bytes;
    }
};
inline void account(Budget& b,const Counts& n,bool borrowedWorld=false){
    b.add<Storage>(1);b.add<Type2StaticCandidate49X*>(n.terrain);
    b.add<std::uint32_t>(n.terrain);b.add<ZoneMethod50CL>(n.terrain);b.add<ContactMaterial50HI>(n.terrain);
    b.add<CandidateHandle50CB>(n.terrain);b.add<Type3Candidate48P>(n.terrain,2);
    b.add<TerrainCandidate48J*>(n.terrain);b.add<TerrainCandidate48J>(n.selected);
    b.add<TerrainCandidateEffects48J>(n.selected,2);b.add<std::size_t>(n.selected,2);
    b.add<std::size_t>(sourceSelectionCapacity);b.add<SearchStorage50BS>(n.methods,2);
    b.add<ZoneRegistration50CJ>(n.objects);b.add<SearchListBlock50BR>(n.blocks);
    b.add<CollectionTree50BX>(n.methods);b.add<CollectionZone50BX>(n.zones);
    b.add<ZoneRegistrationCommand50CQ>(n.events);
    if(!borrowedWorld){b.add<LiveZonePortalInput50EN>(n.portals);
        b.add<CollectionRef50BX>(n.zones);b.add<LiveZonePortalLink50EN>(n.portalLinks);
        b.add<PortalGeometry50EO>(n.portals);}
    b.add<LiveZonePortalLink50EN>(n.portalLinks); // private transaction scratch in either route
    //Scratch, plus the three legacy fixed memberships (legacy layout only).
    const std::size_t memberships=n.sharedRosterCount?1u:4u;
    b.add<CollectionZoneLink50BX>(n.membership,memberships);
    b.add<LiveZoneEntry50EN>(n.membership,memberships);b.add<CollectionObject50BX>(n.objects);
    b.add<CollectionObjectState50BX>(n.objects,4);b.add<CollectionTreeState50BX>(n.methods,4);
    b.add<std::uint32_t>(sourceSelectionCapacity,5);
}
inline bool vectorField(const a::ParameterBlock48F& block,const char* key,Vector3_48C& out){
    const std::string* value=nullptr;
    for(const auto& f:block.fields)if(f.name==key){if(value)return false;value=&f.value;}
    if(!value)return false;
    std::size_t begin=0;
    for(unsigned axis=0;axis<3;++axis){
        auto end=value->find(',',begin);
        if(axis<2&&end==std::string::npos)return false;
        if(axis==2&&end!=std::string::npos)return false;
        if(end==std::string::npos)end=value->size();
        auto a=begin,z=end;while(a<z&&(*value)[a]==' ')++a;while(z>a&&(*value)[z-1]==' ')--z;
        if(!rr::assets::parseTrackNumber50AD(value->data()+a,z-a,out.v[axis]))return false;
        begin=end+1;
    }
    return begin==value->size()+1;
}
inline void reserveFilter(CollisionFilter48J& f,const std::array<std::size_t,3>& maximum){
    f.categoryMasks.reserve(maximum[0]);
    f.identityLists[0].identities.reserve(maximum[1]);
    f.identityLists[1].identities.reserve(maximum[2]);
}
} // namespace detail

// A non-owning view, admitted only inside Owner's private construction path.
// Authentication/lifetime comes from the existing Ground or the RA owner lease.
template<class CollisionOwner> struct SourceSpan {
    const CollisionOwner* values=nullptr;std::size_t count=0;
    std::size_t size()const noexcept{return count;}
    const CollisionOwner& operator[](std::size_t i)const noexcept{return values[i];}
};
template<class CollisionOwner> struct SourceWorldView {
    const a::World48F& world;
    SourceSpan<CollisionOwner> owners;
};
class Owner {
    friend class ra_static_registry50LA::ColdStage;
    friend class ra_source_camera50LA::OwnershipBridge;
    std::unique_ptr<Storage> storage_;
public:
    Owner()=default;
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
    Storage* get() noexcept{return storage_.get();}
    const Storage* get()const noexcept{return storage_.get();}
    // Transactional publication. A failure leaves this owner unprepared and
    // every original/candidate input untouched. No live retry/resizing exists.
    // Caller keeps Ground, candidates, names, filters and radii at their stable
    // addresses longer than this owner; its hot helpers never discover assets.
    bool prepare(std::shared_ptr<OriginalApplicationEntities50LA> app,const Ground& ground,const Binding* bindings,std::size_t count,
        const Limits& limits,Diagnostic& diagnostic) noexcept {
        using Original=typename std::decay_t<decltype(ground.owners)>::value_type;
        const SourceWorldView<Original> source{ground.world,{ground.owners.data(),ground.owners.size()}};
        return prepareSource(source,bindings,count,limits,diagnostic,nullptr,{},std::move(app),limits.roster);
    }
    //Stage S: the retained per-racer collision roster every storage caller
    //passes to the ONE shared-roster factory (rows are sized from it).
    using SharedRosterPlan=RosterPlan;
    //Stage S: owned-world entry of the one shared-roster factory, for the
    //course world owners that build their own portal world (every course but
    //RA, which borrows its World through prepareSharedRosterSource). Same
    //admission, same layout, same bounds; nothing is course-keyed.
    bool prepareShared(std::shared_ptr<OriginalApplicationEntities50LA> app,const Ground& ground,
        const Binding* bindings,std::size_t count,const Limits& limits,const SharedRosterPlan& roster,
        Diagnostic& diagnostic) noexcept {
        using Original=typename std::decay_t<decltype(ground.owners)>::value_type;
        const SourceWorldView<Original> source{ground.world,{ground.owners.data(),ground.owners.size()}};
        return prepareSource(source,bindings,count,limits,diagnostic,nullptr,{},std::move(app),&roster);
    }
private:
    template<class CollisionOwner>
    bool prepareSharedRosterSource(const SourceWorldView<CollisionOwner>& ground,const Binding* bindings,
        std::size_t count,const Limits& limits,Diagnostic& diagnostic,const LiveZoneWorld50EN& borrowed,
        std::shared_ptr<void> lease,std::shared_ptr<OriginalApplicationEntities50LA> app,
        const SharedRosterPlan& roster)noexcept {
        return prepareSource(ground,bindings,count,limits,diagnostic,&borrowed,std::move(lease),std::move(app),&roster);
    }
    template<class CollisionOwner>
    bool prepareSource(const SourceWorldView<CollisionOwner>& ground,const Binding* bindings,std::size_t count,
        const Limits& limits,Diagnostic& diagnostic,const LiveZoneWorld50EN* borrowed,
        std::shared_ptr<void> lease,std::shared_ptr<OriginalApplicationEntities50LA> app,
        const SharedRosterPlan* shared=nullptr) noexcept {
        using namespace detail;
        try {
            diagnostic={};need(!storage_,"track storage already prepared");
            need(app&&app->application10(),"actual live Application collision authority required");
            need(limits.terrainOwners&&limits.terrainOwners<=hardTerrainOwners&&
                limits.zones&&limits.zones<=hardZones&&limits.portals<=hardPortals&&
                limits.coldPayloadBytes&&limits.coldPayloadBytes<=32u*1024u*1024u,
                "track storage limits may only reduce hard ceilings");
            need((!ground.owners.size()||ground.owners.values)&&bindings&&count&&count>=ground.owners.size()&&count<=limits.terrainOwners,
                "track storage requires every original loaded COL owner exactly once");
            if(shared){
                //Borrowed (RA) or owned (prepareShared) World; a borrowed World
                //still needs its exclusive lease (checked again before allocation).
                need(bool(borrowed)==bool(lease)&&shared->owners&&shared->count&&shared->count<=Storage::maximumRoster&&
                    limits.originalRecoveryQueries,"shared roster requires a retained roster, recovery rows and, if borrowed, the exclusive World lease");
                for(std::size_t i=0;i<shared->count;++i){
                    need(shared->owners[i],"missing retained collision owner");
                    for(std::size_t j=0;j<i;++j)need(shared->owners[j]!=shared->owners[i],"duplicate retained collision owner");
                }
            }
            Counts n{};n.sharedRosterCount=shared?shared->count:0;n.terrain=count;n.zones=ground.world.zones.size();n.portals=ground.world.portals.size();
            need(n.zones&&n.zones<=limits.zones&&n.portals<=limits.portals,"original world exceeds storage bounds");
            n.methods=n.zones*2;n.blocks=n.zones*4;
            n.legacyRecoveryReferences=!shared&&limits.originalRecoveryQueries?runtimeRecoveryReferenceReserve:0u;
            const std::size_t cameraReferences=limits.originalCameraQueries?3u:0u;
            const std::size_t roleReferences=shared?shared->count*Storage::rosterRoles:
                runtimeDynamicReferenceReserve+n.legacyRecoveryReferences;
            const std::size_t existingReferences=roleReferences+cameraReferences;
            need(limits.wreckPieceQueries<=(runtimeSearchSlotLimit-existingReferences)/3u,
                "actual wreck query references exceed the one-block SearchList ceiling");
            n.wreckReferences=limits.wreckPieceQueries*3u;
            const std::size_t references=existingReferences+n.wreckReferences;
            need(count<=runtimeSearchSlotLimit-references,
                "actual camera/recovery/wreck query references exceed the one-block SearchList ceiling");
            n.objects=count+references;n.portalLinks=n.portals*2;
            if(n.wreckReferences)n.wreckFirstQuery=static_cast<CollectionRef50BX>(count+existingReferences+1u);
            n.membership=n.portals+1;n.selected=std::max(count,sourceSelectionCapacity);
            n.events=std::max<std::size_t>(128,n.objects);
            n.legacyMainQuery=static_cast<CollectionRef50BX>(count+1);
            n.legacySecondaryQuery=static_cast<CollectionRef50BX>(count+2);
            n.legacyTerrainQuery=static_cast<CollectionRef50BX>(count+3);
            if(!shared&&limits.originalCameraQueries){
                n.cameraSphere=static_cast<CollectionRef50BX>(count+4+n.legacyRecoveryReferences);
                n.cameraRay=static_cast<CollectionRef50BX>(count+5+n.legacyRecoveryReferences);
                n.cameraDeath=static_cast<CollectionRef50BX>(count+6+n.legacyRecoveryReferences);
            }
            if(!shared&&limits.originalRecoveryQueries){
                n.legacyPodQuery=static_cast<CollectionRef50BX>(count+runtimeDynamicReferenceReserve+1u);
                n.legacyMovementQuery=static_cast<CollectionRef50BX>(count+runtimeDynamicReferenceReserve+2u);
            }
            if(shared){
                // Legacy fixed-role consumers cannot mistake reservation for
                // publication. Real per-row refs live only in typed custody.
                n.legacyMainQuery=n.legacySecondaryQuery=n.legacyTerrainQuery=0;
                if(limits.originalCameraQueries){
                    n.cameraSphere=static_cast<CollectionRef50BX>(count+roleReferences+1u);
                    n.cameraRay=static_cast<CollectionRef50BX>(count+roleReferences+2u);
                    n.cameraDeath=static_cast<CollectionRef50BX>(count+roleReferences+3u);
                }
            }
            Budget budget{limits.coldPayloadBytes};account(budget,n,borrowed!=nullptr);
            if(shared){
                budget.add<Storage::SharedRosterAllocation>(1);
                budget.add<Storage::SharedRosterObject>(roleReferences);
                budget.add<live_terrain_probe50fr::Owner>(shared->count);
                budget.add<native_terrain_result50LA::Owner>(shared->count);
                //One actual retained chariot allocation per shared racer.
                //Reservation only; later model publication supplies Entity9C.
                budget.add<NativeRacerTerrainOwner50LA>(shared->count);
                budget.add<NativeRacerAeroOwner50LA>(shared->count);
                budget.add<CollectionZoneLink50BX>(n.membership,roleReferences);
                budget.add<LiveZoneEntry50EN>(n.membership,roleReferences);
            }
            std::array<std::size_t,3> filterMaximum{};
            for(std::size_t z=0;z<n.zones;++z){
                diagnostic.zone=z;const auto choice=a::readZoneSearchSelection50CN(ground.world,z);
                need(ground.world.zones[z].zone==z+1&&choice.status==a::ZoneSearchReadStatus50CN::Complete&&
                    choice.selection.collisionStatic118==a::ZoneSearchKind50CN::List&&
                    choice.selection.collisionDynamic11C==a::ZoneSearchKind50CN::List,
                    "original zone requires unsupported search method; no List substitution");
            }
            std::size_t staticCount=0;
            for(std::size_t i=0;i<count;++i){
                diagnostic.candidate=i;const auto& binding=bindings[i];
                need(binding.candidate,"invalid original collider binding");
                for(std::size_t j=0;j<i;++j)need((binding.animated||binding.trigger||bindings[j].animated||bindings[j].trigger||bindings[j].originalOwner!=binding.originalOwner)&&
                    bindings[j].candidate!=binding.candidate,"duplicate original collider binding");
                const auto& c=binding.candidate->candidate.collision;
                need((!binding.candidate->nativeTriggersMC&&!binding.candidate->nativeTriggersGB)||binding.trigger,"GB source group cannot annotate a non-trigger collider");
                if(binding.trigger){
                    // Native group-owned MA boxes share the existing authored
                    // trigger registry. Legacy Gate bindings remain sphere5;
                    // accepting a box never aliases it to a sphere or mesh.
                    const auto& target=*binding.candidate;
                    const bool sphere=target.shapeType==5&&(bool(target.gate)!=bool(target.nativeTriggers));
                    const bool box=target.shapeType==6&&!target.gate&&target.nativeTriggers&&
                        validNativeTriggerBox50MA(target.nativeTriggerBox)&&
                        !std::memcmp(c.geometry.worldMatrix.v,target.nativeTriggerBox.world.v,sizeof c.geometry.worldMatrix.v);
                    // TC is a distinct cold group. Its authentic aggregate
                    // joins the actual prepared Action/source/Sound owners;
                    // late racer identity binding intentionally has not made
                    // group.ready true at this Storage construction point.
                    bool tc=false;
                    if(target.nativeTriggersTC&&!target.nativeTriggersMC&&!target.nativeTriggersTB&&!target.nativeTriggersGB&&!target.gate&&!target.nativeTriggers){
                        const auto& group=*target.nativeTriggersTC;
                        if(target.nativeTriggerIndex<group.triggers.size()){
                            const auto& state=group.triggers[target.nativeTriggerIndex];
                            const bool shape=target.shapeType==5||(target.shapeType==6&&
                                validNativeTriggerBox50MA(target.nativeTriggerBox)&&
                                !std::memcmp(c.geometry.worldMatrix.v,target.nativeTriggerBox.world.v,sizeof c.geometry.worldMatrix.v));
                            tc=shape&&!group.ready&&!group.queryActive&&!group.actionsFailed&&
                                group.actions.context&&group.actions.preflight&&group.actions.resume&&
                                state.collider.identity==c.filter.identity&&
                                state.randomTriggerPercent90==100&&
                                state.callbackCount98==(target.nativeTriggerIndex==10?3u:1u)&&
                                state.collider.highWaterD0==0;
                        }
                    }
                    bool tb=false;
                    if(target.nativeTriggersTB&&!target.nativeTriggersMC&&!target.nativeTriggersTC&&!target.nativeTriggersGB&&!target.gate&&!target.nativeTriggers){
                        const auto& group=*target.nativeTriggersTB;
                        if(target.nativeTriggerIndex<group.triggers.size()){
                            const auto& state=group.triggers[target.nativeTriggerIndex];
                            const bool shape=target.shapeType==5||(target.shapeType==6&&
                                validNativeTriggerBox50MA(target.nativeTriggerBox)&&
                                !std::memcmp(c.geometry.worldMatrix.v,target.nativeTriggerBox.world.v,sizeof c.geometry.worldMatrix.v));
                            tb=shape&&!group.ready&&!group.queryActive&&!group.actionsFailed&&
                                group.actions.context&&group.actions.preflight&&group.actions.resume&&
                                state.collider.identity==c.filter.identity&&
                                state.randomTriggerPercent90==100&&
                                state.callbackCount98==(target.nativeTriggerIndex==15?2u:1u)&&
                                state.collider.highWaterD0==0;
                        }
                    }
                    bool mc=false;
                    if(target.nativeTriggersMC&&!target.nativeTriggersTB&&!target.nativeTriggersTC&&!target.nativeTriggersGB&&!target.gate&&!target.nativeTriggers){
                        const auto& group=*target.nativeTriggersMC;
                        if(target.nativeTriggerIndex<group.triggers.size()){
                            const auto& state=group.triggers[target.nativeTriggerIndex];
                            const bool shape=target.shapeType==5||(target.shapeType==6&&
                                validNativeTriggerBox50MA(target.nativeTriggerBox)&&
                                !std::memcmp(c.geometry.worldMatrix.v,target.nativeTriggerBox.world.v,sizeof c.geometry.worldMatrix.v));
                            mc=shape&&!group.ready&&!group.queryActive&&!group.actionsFailed&&
                                group.actions.context&&group.actions.preflight&&group.actions.resume&&
                                state.collider.identity==c.filter.identity&&
                                state.randomTriggerPercent90==(target.nativeTriggerIndex==20?50:(target.nativeTriggerIndex==1||target.nativeTriggerIndex==2||target.nativeTriggerIndex==19)?100:99)&&
                                state.callbackCount98==1&&
                                state.collider.highWaterD0==0;
                        }
                    }
                    // GB is a distinct exact 18-row cold group. The aggregate
                    // must separately prove same-source ActionOwner ownership;
                    // this storage check neither supplies hooks nor grants ready.
                    bool gb=false;
                    if(target.nativeTriggersGB&&!target.nativeTriggersMC&&!target.nativeTriggersTB&&!target.nativeTriggersTC&&!target.gate&&!target.nativeTriggers){
                        const auto& group=*target.nativeTriggersGB;
                        if(target.nativeTriggerIndex<group.triggers.size()){
                            const auto& state=group.triggers[target.nativeTriggerIndex];
                            gb=target.shapeType==5&&!group.ready&&!group.queryActive&&!group.actionsFailed&&
                                group.actions.context&&group.actions.preflight&&group.actions.resume&&
                                state.collider.identity==c.filter.identity&&state.randomTriggerPercent90==100&&
                                state.callbackCount98==(target.nativeTriggerIndex==15?2u:target.nativeTriggerIndex==17?5u:1u)&&
                                state.collider.highWaterD0==0;
                        }
                    }
                    need(!binding.animated&&binding.originalOwner==std::numeric_limits<std::size_t>::max()&&
                        binding.authoredZone&&binding.authoredZone<=n.zones&&binding.method==ZoneMethod50CL::First&&
                        ((!target.nativeTriggersTC&&!target.nativeTriggersMC&&!target.nativeTriggersTB&&!target.nativeTriggersGB&&(sphere||box))||tc||tb||mc||gb)&&c.shapeType==target.shapeType&&
                        !c.geometry.mesh&&!c.compound&&c.hasResponseCallbackE8&&c.filter.categoryA4==8u&&
                        c.filter.identity&&c.geometry.id==c.filter.identity&&c.ownerIdentity9C,
                        "original native sphere/box trigger binding is incomplete");
                }else if(binding.animated){
                    need(binding.originalOwner==std::numeric_limits<std::size_t>::max()&&binding.authoredZone&&binding.authoredZone<=n.zones&&
                        // Every track-animation row (common player, any course) is a compound.
                        binding.candidate->shapeType==1&&binding.candidate->compound&&c.shapeType==1&&c.compound==binding.candidate->compound&&
                        c.filter.identity&&c.geometry.id==c.filter.identity&&c.ownerIdentity9C,
                        "original animated compound binding is incomplete");
                }else{
                need(binding.originalOwner<ground.owners.size(),"invalid original SCB collider index");++staticCount;
                const auto& original=ground.owners[binding.originalOwner];
                need(original.zone&&original.zone<=n.zones&&c.geometry.mesh==&original.mesh&&
                    !std::memcmp(c.geometry.worldMatrix.v,original.instance.world.v,sizeof original.instance.world.v)&&
                    c.filter.categoryA4==original.instance.category&&c.filter.identity&&
                    !std::memcmp(c.sphere.center.v,original.instance.sphere.center.v,sizeof c.sphere.center.v)&&
                    !std::memcmp(&c.sphere.radius,&original.instance.sphere.radius,sizeof(float)),
                    "candidate does not borrow its original prepared COL placement/category/sphere");
                }
                const auto& f=c.filter;
                const std::size_t lengths[]{f.categoryMasks.size(),f.identityLists[0].identities.size(),f.identityLists[1].identities.size()};
                for(unsigned k=0;k<3;++k){budget.add<std::uint32_t>(lengths[k]);filterMaximum[k]=std::max(filterMaximum[k],lengths[k]);}
            }
            need(staticCount==ground.owners.size(),"not every original SCB collider is bound exactly once");
            for(auto maximum:filterMaximum){budget.add<std::uint32_t>(maximum,n.terrain);budget.add<std::uint32_t>(maximum,n.selected);}
            diagnostic.requestedPayloadBytes=budget.used;
            need(bool(borrowed)==bool(lease),"borrowed world requires its exclusive retained lease");
            if(borrowed)need(borrowed->zoneCount==n.zones&&borrowed->portalCount==n.portals&&
                borrowed->portalLinkCount==n.portalLinks&&borrowed->portalHeads&&
                (!n.portalLinks||borrowed->portalLinks)&&(!n.portals||borrowed->portals),
                "borrowed source portal world extent mismatch");
            auto next=std::unique_ptr<Storage>(new Storage(n,borrowed,std::move(lease),std::move(app),shared!=nullptr));
            if(shared){
                next->collectionInput.objectCount=n.terrain;
                next->sharedRoster_=std::make_unique<Storage::SharedRosterAllocation>();
                next->sharedRoster_->count=shared->count;
                next->sharedRoster_->coldPayloadRemaining=budget.capacity-budget.used;
                for(std::size_t slot=0;slot<shared->count;++slot)
                    for(std::size_t role=0;role<Storage::rosterRoles;++role){
                        const auto index=slot*Storage::rosterRoles+role;
                        next->sharedRoster_->objects[index]=std::make_unique<Storage::SharedRosterObject>(
                            *shared->owners[slot],slot,static_cast<Storage::RosterRole>(role),
                            static_cast<CollectionRef50BX>(count+index+1u),n.membership);
                        if(role==static_cast<std::size_t>(Storage::RosterRole::terrain348)){
                            next->sharedRoster_->objects[index]->terrainResource=std::make_unique<live_terrain_probe50fr::Owner>();
                            next->sharedRoster_->objects[index]->terrainResult90=std::make_unique<native_terrain_result50LA::Owner>();
                        }
                    }
            }
            for(std::size_t i=0;i<count;++i){
                diagnostic.candidate=i;auto& c=*bindings[i].candidate;
                const auto& binding=bindings[i];
                next->candidates[i]=&c;next->candidateZones[i]=(binding.animated||binding.trigger)?binding.authoredZone:ground.owners[binding.originalOwner].zone;
                next->candidateMethods[i]=binding.method;
                next->terrainTargets[i]=&c.candidate.collision;
                next->handles[i]={c.candidate.collision.filter.identity,i};
                std::string error;
                if(binding.animated||binding.trigger)next->contactMaterials[i]={c.candidate.collision.filter.identity,binding.materialA9};
                else {const auto& original=ground.owners[binding.originalOwner];
                    need(a::readTrackContactMaterial50HI(ground.world,original.zone,original.ordinal,
                        c.candidate.collision.filter.identity,next->contactMaterials[i],error),
                        "original contact MaterialID mapping rejected");}

                auto& movement=next->movement[i];movement.collision=c.candidate.collision;
                movement.shapeType=c.shapeType;movement.compound=c.compound;movement.hasMaterialScene=c.candidate.hasMaterialScene;
                movement.sceneName=c.candidate.sceneName;movement.sceneNameBytes=c.candidate.sceneNameBytes;
                reserveFilter(next->registeredMovement[i].collision.filter,filterMaximum);
                const auto p=projectCollectionObject50CF(c.candidate.collision.filter,
                    c.candidate.collision.sphere,c.candidate.collision.typeTag,c.shapeType,0);
                need(p.valid,"original candidate collector projection rejected");
                next->objects[i]=p.object;next->source[i]=p.state;
            }
            std::sort(next->handles.begin(),next->handles.end(),[](const auto& x,const auto& y){return x.identity<y.identity;});
            for(std::size_t i=1;i<count;++i)need(next->handles[i-1].identity!=next->handles[i].identity,
                "duplicate original candidate identity");
            need(live_terrain_probe50fr::reserveSelected(next->terrainSelected.data(),n.selected,
                next->terrainTargets.data(),count),"reserve real terrain filter copies");
            need(initializeColdSearchStorage50BU(next->movementRegistry,runtimeSearchSlotLimit)==SearchStorageStatus50BS::Complete,
                "movement SearchList initialization failed");
            std::uint32_t slot=0;
            need(adoptFirstSearchBlock50BU(next->movementRegistry,slot)==SearchStorageStatus50BS::Complete,
                "movement SearchList first block unavailable");
            for(std::size_t i=0;i<count;++i){
                need(selectSearchSlot50BS(next->movementRegistry,0,slot)==SearchStorageStatus50BS::Complete&&slot==i+1&&
                    insertSearchSlot50BS(next->movementRegistry,slot,static_cast<std::uint32_t>((i+1)*4))==SearchStorageStatus50BS::Complete,
                    "original ordered movement registry rejected");
            }
            const ZoneLifecycleStorage50CL zones{next->registries.data(),n.methods,
                next->registrations.data(),n.objects,{next->trees.data(),n.methods,
                next->zones.data(),n.zones,next->blocks.data(),n.blocks}};
            need(initializeZoneLifecycle50CL(next->lifecycle,zones,n.zones,n.objects,runtimeSearchSlotLimit,
                next->registrationScratch)==SearchStorageStatus50BS::Complete,"track zone lifecycle initialization failed");
            next->queue={next->events.data(),n.events,0,0};
            for(std::size_t i=0;i<count;++i)if(!bindings[i].animated&&!bindings[i].trigger)
                need(enqueueZoneRegistration50CQ(next->queue,{ZoneRegistrationOperation50CQ::Set,
                    static_cast<CollectionRef50BX>(i+1),next->candidateZones[i],next->candidateMethods[i]})==ZoneQueueStatus50CQ::Complete,
                    "cold original collider registration queue full");
            const auto drained=drainZoneRegistrations50CQ(next->queue,next->lifecycle,next->registrationScratch,n.events);
            need(drained.status==ZoneQueueStatus50CQ::Complete&&!drained.remaining,"cold original collider registration rejected");
            if(!borrowed){
            for(std::size_t i=0;i<n.portals;++i){
                const auto& p=ground.world.portals[i];need(p.parameter_block<ground.world.parameters.blocks.size(),"invalid original portal block");
                auto& v=next->authoredPortals[i];v.zoneA=p.zones[0];v.zoneB=p.zones[1];
                const auto& block=ground.world.parameters.blocks[p.parameter_block];
                for(unsigned k=0;k<4;++k)need(vectorField(block,("vertex"+std::to_string(k+1)).c_str(),v.vertices[k]),
                    "original portal vertex text rejected");
            }
            const LiveZoneWorldStorage50EN portals{next->portalHeads.data(),n.zones,
                next->portalLinks.data(),n.portalLinks,next->portalGeometry.data(),n.portals};
            need(prepareLiveZoneWorld50EN(next->authoredPortals.data(),n.portals,n.zones,portals,next->world)==
                LiveZoneStatus50EN::Complete,"original portal world preparation rejected");
            }
            next->membershipWork={next->membershipScratchRegistries.data(),n.methods,
                next->membershipScratchLinks.data(),next->membershipScratchEntries.data(),n.membership,
                next->portalScratchLinks.data(),n.portalLinks,{}};
            auto& input=next->collectionInput;input.objects=next->objects.data();
            // Recovery reserves a contiguous pair beside the three required
            // vehicle owners. None may be queried before all cold bindings
            // publish actual records. The camera expands this BANKED active
            // extent only in bank0 after its own trailing pair is published.
            //Shared: the static prefix plus every racer's five rows (activeExtent).
            //An unattached row is in no zone list, so the collector never meets it.
            input.objectCount=next->activeExtent(false);
            // Shared reserved rows are not active, described, attached, or
            // visible merely because storage exists. Stage C owns publication.
            input.trees=next->trees.data();input.treeCount=n.methods;input.zones=next->zones.data();input.zoneCount=n.zones;
            // Query owner, bounds, actual E0 membership and type tags remain
            // explicitly unbound until the caller has prepared each real query.
            storage_=std::move(next);diagnostic.message.clear();return true;
        }catch(const std::exception& e){try{diagnostic.message=e.what();}catch(...){}return false;}
        catch(...){try{diagnostic.message="Unexpected cold track-storage failure";}catch(...){}return false;}
    }
};
} // namespace track_runtime_storage50IG

#pragma once
#include "live_zone_membership50en.h"
#include "vehicle_indexed_query50cd.h"
#include "vehicle_secondary_contact50j.h"

namespace rr::game {
// Native nonnull E0 projection, with independent ownership for each query.
// expectedObjectRef belongs to the caller; it is not inferred from membership.
struct VehicleLiveQueryContext50EQ {
    ZoneLifecycle50CL& lifecycle;
    LiveZoneWorld50EN& world;
    LiveZoneMembership50EN& registrationE0;
    LiveZoneWorkspace50EN& workspace;
    CollectionRef50BX expectedObjectRef;
};
enum class VehicleLiveQueryStatus50EQ {
    Complete,AdmissionRejected,PreparationRejected,PrefixRejected,
    ReceiptRejected,QueryRejected
};
struct VehicleLiveQueryResult50EQ {
    VehicleLiveQueryStatus50EQ status=VehicleLiveQueryStatus50EQ::AdmissionRejected;
    Type2StaticStatus49X queryStatus=Type2StaticStatus49X::InvalidInput;
    LiveZoneResult50EN prefix{};
    bool prefixCommitted=false;
    std::uint64_t membershipRevision=0;
};

// Registered counterpart of the50J secondary getter. Call AFTER the main query
// and BEFORE the secondary live prefix. Refreshes the canonical physical scene
// and matching body cache in source order; does not update membership or query.
// A completed getter remains committed if the later live prefix/query rejects.
// Requires the secondary query's own attached E0/object; legacy50J is unchanged.
SecondaryQuerySceneStatus50J prepareSecondaryLiveQueryScene50EQ(
    VehicleFrameState49W&,const VehicleFrameBindings49W&,
    const MainStaticDetectorAdmission50H&,const VehicleLiveQueryContext50EQ&,
    VehicleFrameWorkspace49W&) noexcept;

// Registered null-pair295630 route: current11FE50/11F680 preparation, then
//120230 ->1202E0 ->280D40, then295A08/122660 over the ALREADY collected list.
// Source219AD4 collects through main spring358->38 BEFORE either spring call.
// Caller must finish that collection and resolve its indices before this call;
// never recollect here or substitute the secondary membership for the main one.
// Outer122350 uses the prior query matrix/cached sphere as well as prior E0;
//1202E0 recomputes its type2 AABB without refreshing scene, matrix or sphere.
// Main/secondary require separate attached memberships and expected object refs.
// admission.registrationE0 remains a nonzero presence marker, not a guest address;
// the typed registrationE0 reference supplies the actual native live state.
// All other50CD phase/body/scene/pair/provider gates are retained unchanged.
//
// The prepared scene/resource/object/revision/cache receipt is checked after a
// successful live update, before49X. A later query rejection preserves committed
// live prefix effects; destination/result retain49X's transactional guarantees.
// Admission/preparation/prefix rejection does not commit a live update. Scratch
// can change on prefix/query failure. No whole-source transaction is claimed.
// Full declared live/query writable spans must be mutually disjoint and disjoint
// from all inputs; calls are serial with no active collection cursor. Existing
//50EN ownership/lifetime/bounds and49X immutable-resource contracts also apply.
VehicleLiveQueryResult50EQ queryMainLiveIndexedStaticVehicle50EQ(
    const VehicleFrameState49W&,const VehicleFrameBindings49W&,
    const MainStaticDetectorAdmission50H&,const VehicleLiveQueryContext50EQ&,
    const Type2StaticQuery49X&,const Type2StaticCandidate49X*,std::size_t ownerCount,
    const std::size_t* indices,std::size_t count,const Type2StaticInitial49X&,
    const Type2StaticWorkspace49X&,const Type2StaticDestination49X&,
    Type2StaticResult49X&) noexcept;
VehicleLiveQueryResult50EQ querySecondaryLiveIndexedStaticVehicle50EQ(
    const VehicleFrameState49W&,const VehicleFrameBindings49W&,
    const MainStaticDetectorAdmission50H&,const VehicleLiveQueryContext50EQ&,
    const Type2StaticQuery49X&,const Type2StaticCandidate49X*,std::size_t ownerCount,
    const std::size_t* indices,std::size_t count,const Type2StaticInitial49X&,
    const Type2StaticWorkspace49X&,const Type2StaticDestination49X&,
    Type2StaticResult49X&) noexcept;
}
